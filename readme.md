# Double Agent
### Android

make this alteration to ofxNetworking:

```
bool ofxTCPManager::Bind(unsigned short usPort)
{
	struct sockaddr_in local;
	memset(&local, 0, sizeof(sockaddr_in));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	//Port MUST be in Network Byte Order
	local.sin_port = htons(usPort);
    int yes=1;
    // lose the pesky "Address already in use" error message
    if (setsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
	if (bind(m_hSocket,(struct sockaddr*)&local,sizeof(local))){
		ofxNetworkCheckError();
		return false;
	}
	return true;
}
```