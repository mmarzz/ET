#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "Rpc/RpcSession.h"
#include "Rpc/RpcServer.h"

namespace Egametang {

RpcSession::RpcSession(boost::asio::io_service& ioService, RpcServer& server):
		RpcCommunicator(ioService), rpcServer(server)
{
}

RpcSession::~RpcSession()
{
}

void RpcSession::OnRecvMessage(RpcMetaPtr meta, StringPtr message)
{
	RpcSessionPtr session = shared_from_this();
	rpcServer.RunService(session, meta, message,
			boost::bind(&RpcSession::SendMeta, session, _1, _2));

	// 可以循环利用
	RecvMeta(meta, message);
}

void RpcSession::OnSendMessage(RpcMetaPtr meta, StringPtr message)
{
}

void RpcSession::Start()
{
	RpcMetaPtr meta = boost::make_shared<RpcMeta>();
	StringPtr message = boost::make_shared<std::string>();
	RecvMeta(meta, message);
}

void RpcSession::Stop()
{
	RpcSessionPtr session = shared_from_this();

	// 延迟删除,必须等所有的bind执行完成后才能remove,
	// 否则会出现this指针失效的问题
	ioService.post(boost::bind(&RpcServer::Remove, &rpcServer, session));
}

} // namespace Egametang
