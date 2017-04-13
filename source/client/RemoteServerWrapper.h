#ifndef SRC_REMOTESERVERWRAPPER_H_
#define SRC_REMOTESERVERWRAPPER_H_

class IConnection;

class RemoteServerWrapper final {
public:
	explicit RemoteServerWrapper(IConnection & server);
	~RemoteServerWrapper() = default;

	void playerLeft();
	void playerRight();
	void playerForward();
	void playerBackward();
	void playerStops();

	IConnection & conn() { return server; }
private:
	IConnection & server;
};

#endif /* SRC_REMOTESERVERWRAPPER_H_ */
