#include "config.h"
#ifdef USE_UDNS

#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <udns.h>

#include <torrent/common.h>
#include "udnsevent.h"
#include "globals.h"
#include "manager.h"
#include "torrent/poll.h"

namespace torrent {

const int udnserror_to_gaierror[] = {
	EAI_AGAIN,
	EAI_AGAIN,
	EAI_FAIL,
	EAI_NONAME,
	EAI_ADDRFAMILY,
	EAI_MEMORY,
	EAI_NONAME	
};

// Compatibility layers so udns can call std::function callbacks.
void a4_callback_wrapper(struct ::dns_ctx *ctx, ::dns_rr_a4 *result, void *data) {
  udns_query *query = static_cast<udns_query*>(data);
  // udns will free the a4_query after this callback exits
  query->a4_query = NULL;

  if (result == NULL || result->dnsa4_nrr == 0) {
    // nothing more to do: call the callback with a failure status
    (*(query->callback))(NULL, udnserror_to_gaierror[abs(::dns_status(ctx))]);
    delete query;
    // else: return and wait to see if we get an a6 response
  } else {
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = 0;
    sa.sin_addr = result->dnsa4_addr[0];
    (*query->callback)(&sa, 0);
    delete query;
  }
}

UdnsEvent::UdnsEvent() {
  // reinitialize the default context, no-op
  // TODO don't do this here --- do it once in the manager, or in rtorrent
  ::dns_init(NULL, 0);
  // thread-safe context isolated to this object:
  m_ctx = ::dns_new(NULL);
  m_fileDesc = ::dns_open(m_ctx);
  if (m_fileDesc == -1) throw internal_error("dns_init failed");

  m_taskTimeout.slot() = std::bind(&UdnsEvent::process_timeouts, this);
}

UdnsEvent::~UdnsEvent() {
  priority_queue_erase(&taskScheduler, &m_taskTimeout);
  ::dns_close(m_ctx);
  ::dns_free(m_ctx);
  m_fileDesc = -1;

  for (auto it = std::begin(m_malformed_queries); it != std::end(m_malformed_queries); ++it) {
    delete *it;
  }
}

void UdnsEvent::event_read() {
  ::dns_ioevent(m_ctx, 0);
}

void UdnsEvent::event_write() {
}

void UdnsEvent::event_error() {
}

struct udns_query *UdnsEvent::enqueue_resolve(const char *name, resolver_callback *callback) {
  struct udns_query *query = new udns_query { NULL, callback, 0 };

  query->a4_query = ::dns_submit_a4(m_ctx, name, 0, a4_callback_wrapper, query);
  if (query->a4_query == NULL) {
    // XXX udns does query parsing up front and will fail immediately
    // during submission of malformed domain names, e.g., `..`. In order to
    // maintain a clean interface, keep track of this query internally
    // so we can call the callback later with a failure code
    if (::dns_status(m_ctx) == DNS_E_BADQUERY) {
      // this is what getaddrinfo(3) would return:
      query->error = EAI_NONAME;
      m_malformed_queries.push_back(query);
      return query;
    } else {
      // unrecoverable errors, like ENOMEM
      throw new internal_error("dns_submit_a4 failed");
    }
  }

  return query;
}

void UdnsEvent::flush_resolves() {
  // first process any queries that were malformed
  while (!m_malformed_queries.empty()) {
    udns_query *query = m_malformed_queries.back();
    m_malformed_queries.pop_back();
    (*(query->callback))(NULL, query->error);
    delete query;
  }
  process_timeouts();
}

void UdnsEvent::process_timeouts() {
  int timeout = ::dns_timeouts(m_ctx, -1, 0);
  if (timeout == -1) {
    // no pending queries
    manager->poll()->remove_read(this);
    manager->poll()->remove_error(this);
  } else {
    manager->poll()->insert_read(this);
    manager->poll()->insert_error(this);
    priority_queue_upsert(&taskScheduler, &m_taskTimeout, (cachedTime + rak::timer::from_seconds(timeout)).round_seconds());
  }
}

void UdnsEvent::cancel(struct udns_query *query) {
  if (query == NULL) return;

  if (query->a4_query != NULL) ::dns_cancel(m_ctx, query->a4_query);

  auto it = std::find(std::begin(m_malformed_queries), std::end(m_malformed_queries), query);
  if (it != std::end(m_malformed_queries)) m_malformed_queries.erase(it);

  delete query;
}

const char *UdnsEvent::type_name() {
  return "UdnsEvent";
}

}

#endif
