/*
 * ConServer.cpp
 *
 *  Created on: 31/ago/2012
 *      Author: neo
 */

#include <XNet/server/ConServer.hpp>
#include <cassert>
#include <cstring>

namespace XNet {

const unsigned int ConServer::_clientsMAXdefault = 50;
const Uint32 ConServer::_delayCheckSocketsdefault = 0;
const int ConServer::_MAXbufferSizeMessagedefault = 512;	//bytes

ConServer::ConServer(const Uint16 port):_serversocket(NULL), _clientsMAX(ConServer::_clientsMAXdefault), _socketset(NULL),
		_IDmemCount(0),_delayCheckSockets(_delayCheckSocketsdefault), _MAXbufferSizeMessage(_MAXbufferSizeMessagedefault){
	this->Set_PortConnection(port);
}

ConServer::~ConServer() {
	this->CloseConnection();
}


const bool ConServer::Set_PortConnection(const Uint16 constUnsignedInt){
	if(this->_serversocket!=NULL){
		this->_errorLOG+="@Set_PortConnection: Connessione già attiva!\n";
		return false;
	}
	this->_porto_connessione=constUnsignedInt;
	return true;
}

const bool ConServer::OpenConnection(){
	if(this->_serversocket!=NULL){
		this->_errorLOG+="@OpenConnection: Connessione già attiva!\n";
		return false;
	}
	if(SDLNet_ResolveHost(&this->_ipaddress, NULL, this->_porto_connessione)!=0){
		this->_errorLOG+="@OpenConnection: Impossibile effettuare la risoluzione dell'host!\n";
		this->_errorLOG+=SDLNet_GetError();
		this->_errorLOG+='\n';
		return false;
	}
	this->_serversocket=SDLNet_TCP_Open(&this->_ipaddress);
	if(this->_serversocket==NULL){
		this->_errorLOG+="@OpenConnection: Impossibile effettuare una connessione\n";
		this->_errorLOG+=SDLNet_GetError();
		this->_errorLOG+='\n';
		return false;
	}

	this->_socketset = SDLNet_AllocSocketSet(this->_clientsMAX + 1);
	if(this->_socketset ==NULL){
		this->_errorLOG+="@OpenConnection: Impossibile instanzare un set di sockets\n";
		this->_errorLOG+=SDLNet_GetError();
		this->_errorLOG+='\n';
		SDLNet_TCP_Close(this->_serversocket);
		this->_serversocket=NULL;
		return false;
	}

	int _idNewServer=0;
	switch(this->RegisterIncomingSocket(this->_serversocket, _idNewServer)){
	case ERROR:
		SDLNet_FreeSocketSet(this->_socketset);
		this->_socketset=NULL;
		SDLNet_TCP_Close(this->_serversocket);
		this->_serversocket=NULL;
		return false;
		break;
	default:
		break;
	}

	this->_listClientsBye.reserve(this->_clientsMAX);
	this->_listClientsLog.reserve(this->_clientsMAX);
	this->_listClientsProc.reserve(this->_clientsMAX);

	return true;
}

const bool ConServer::Set_MaxUsers(const unsigned int max_users_set){
	if(this->_serversocket!=NULL){
		this->_errorLOG+="@Set_MaxUsers: Connessione già attiva!\n";
		return false;
	}
	this->_clientsMAX=max_users_set;
	return true;
}

void ConServer::CloseConnection() {
	if (this->_serversocket != NULL) {
		HASH_ITERATOR it;
		for(it=this->_mapSockets.begin(); it!=this->_mapSockets.end(); ){
			SDLNet_TCP_DelSocket(this->_socketset,(*it).second);
			if((*it).first!=0){
				SDLNet_TCP_Close((*it).second);
				(*it).second=NULL;
			}
			this->_mapSockets.erase(it);
			it++;
		}
		SDLNet_FreeSocketSet(this->_socketset);
		this->_socketset=NULL;
		SDLNet_TCP_Close(this->_serversocket);
		this->_serversocket = NULL;
		this->_IDmemCount=0;
	}
}



const bool ConServer::Set_MaxBytesBuffer(const int size_buffer){
	if(this->_serversocket==NULL){
		this->_errorLOG+="@Set_MaxBytesBuffer: Impossibile effettuare una configurazione dopo aver aperto la connessione!\n";
		return false;
	}
	this->_MAXbufferSizeMessage=size_buffer;
	return true;
}

const ConServer::REG_RTS_VALUE ConServer::RegisterIncomingSocket(const TCPsocket connection, ID_CLIENT& new_id){
	if(this->_socketset==NULL) return ERROR;
	if(connection==NULL) return ERROR;
	if(this->Get_UsersInServer() >= this->_clientsMAX){
		//TODO: devi inviare un mess al client dicendo che il server è pieno
		return SERVER_FULL;
	}
	if(SDLNet_TCP_AddSocket(this->_socketset, connection)==-1){
		this->_errorLOG+="@RegisterIncomingSocket: Impossibile registrare un socket nel set\n";
		this->_errorLOG+=SDLNet_GetError();
		this->_errorLOG+='\n';
		return ERROR;
	}
	this->_mapSockets.insert(std::pair<ID_CLIENT,TCPsocket>(this->_IDmemCount,connection));
	new_id=this->_IDmemCount;
	this->_IDmemCount++;
	return REGISTERED;
}

const bool ConServer::CloseAConnectionClient(const ID_CLIENT id_erase) {
	if(id_erase==0){
		this->_errorLOG+="@CloseAConnectionClient: Impossibile eliminare il socket server (ID:0)\n";
		return false;
	}
	HASH_ITERATOR it = this->_mapSockets.find(id_erase);
	if(it==this->_mapSockets.end()){
		this->_errorLOG=+"@CloseAConnectionClient: Impossibile eliminare un socket non identificato (ID:";
		this->_errorLOG=+id_erase;
		this->_errorLOG=+")\n";
		return false;
	}
	if(SDLNet_TCP_DelSocket(this->_socketset, (*it).second)==-1){
		this->_errorLOG+="@CloseAConnectionClient: Impossibile eliminare un socket nel set\n";
		this->_errorLOG+=SDLNet_GetError();
		this->_errorLOG+='\n';
		return false;
	}
	SDLNet_TCP_Close((*it).second);
	(*it).second=NULL;
	this->_mapSockets.erase(it);
	this->_mapDataClients.erase(id_erase);
	return true;
}

const std::string ConServer::Get_AddressIpServer(void) const {
	return std::string(SDLNet_ResolveIP(&this->_ipaddress));
}

const int ConServer::UpDateConnection(void(*pthreadNew)(const ID_CLIENT), void(*pthreadDel)(const ID_CLIENT),
		void(*pthreadProc)(const ID_CLIENT, const char*, const int)){
	this->_listClientsBye.clear();
	this->_listClientsLog.clear();
	this->_listClientsProc.clear();
	assert(this->_serversocket);
	int rts_result=NONE;

	DATA_CLIENTS::iterator it_send;
	ID_CLIENT _currentID;
	for(it_send=this->_toSend.begin(); it_send!=this->_toSend.end(); it_send++){
		BUFFER_DATA& _currentData=(*it_send).second;
		_currentID=(*it_send).first;

		size_t bytes_to_send=_currentData.size();
		if(bytes_to_send > 0){
			if(bytes_to_send > (size_t)(this->_MAXbufferSizeMessage)){
				bytes_to_send = this->_MAXbufferSizeMessage;
			}
			char buffer_temp_send[bytes_to_send];
			Funct::MakeMemory_fromVectorChar(buffer_temp_send,bytes_to_send,_currentData);
			if(SDLNet_TCP_Send(this->_mapSockets[_currentID],buffer_temp_send,bytes_to_send) < (int)(bytes_to_send)){
				this->_errorLOG+="@UpDateConnection: Errore in trasmissione del buffer dati.\n";
				this->_errorLOG+=SDLNet_GetError();
				this->_errorLOG+='\n';
				rts_result=rts_result | WITH_ERROR;
			}
			rts_result=rts_result | SEND_DATA;
		}
	}

	int num_sockets_activity=SDLNet_CheckSockets(this->_socketset, this->_delayCheckSockets);
	if(num_sockets_activity!=0){
		if(num_sockets_activity==-1){
			this->_errorLOG+="@UpDateConnection: Impossibile controllare il set.\n";
			this->_errorLOG+=SDLNet_GetError();
			this->_errorLOG+='\n';
			rts_result=rts_result | WITH_ERROR;
		}else{
			char buffer_temp_message[this->_MAXbufferSizeMessage];
			register HASH_ITERATOR it;
			TCPsocket _currentSocket;
			ID_CLIENT _currentID;
			for(it=this->_mapSockets.begin(); it!=this->_mapSockets.end(); it++){
				_currentSocket = (*it).second;
				_currentID = (*it).first;
				if(SDLNet_SocketReady(_currentSocket)!=0){
					if(_currentID==0){
						//c'è una nuova connessione in arrivo sul server
						TCPsocket new_connection=SDLNet_TCP_Accept(this->_serversocket);
						int id_incoming=-1;
						switch(this->RegisterIncomingSocket(new_connection,id_incoming)){
						case ERROR:
							if(new_connection!=NULL){
								this->_errorLOG+="@UpDateConnection: Impossibile effettuare la registrazione di un nuovo socket in arrivo.\n";
								this->_errorLOG+=SDLNet_GetError();
								this->_errorLOG+='\n';
								rts_result=rts_result | WITH_ERROR;
							}
							break;
						case REGISTERED:
							rts_result=rts_result | NEW_CLIENT;
							this->_listClientsLog.push_back(id_incoming);
							if(pthreadNew!=NULL){
								pthreadNew(id_incoming);
							}
							break;
						default:	//no warning plz -.-"
							break;
						}
					}else{
						int bytes_rvc = SDLNet_TCP_Recv(_currentSocket, buffer_temp_message, this->_MAXbufferSizeMessage);
						if(bytes_rvc <= 0){
							if(SDLNet_TCP_DelSocket(this->_socketset, (*it).second)==-1){
								this->_errorLOG+="@UpDateConnection: Impossibile eliminare un socket nel set\n";
								this->_errorLOG+=SDLNet_GetError();
								this->_errorLOG+='\n';
								rts_result=rts_result | WITH_ERROR;
							}
							SDLNet_TCP_Close(_currentSocket);
							_currentSocket=NULL;
							this->_mapSockets.erase(_currentID);
							this->_toSend.erase(_currentID);
							this->_mapDataClients.erase(_currentID);
							rts_result=rts_result | CLIENT_DISCONNECT;
							this->_listClientsBye.push_back(_currentID);
							if(pthreadDel!=NULL){
								pthreadDel(_currentID);
							}
						}else{
							Funct::AddMemory_intoVectorChar(buffer_temp_message,bytes_rvc,this->_mapDataClients[_currentID]);
							rts_result=rts_result | CLIENT_RCVDATA;
							this->_listClientsProc.push_back(_currentID);
							if(pthreadProc!=NULL){
								pthreadProc(_currentID,buffer_temp_message,bytes_rvc);
							}
						}
					}
				}
			}
		}
	}

	return rts_result;
}

const std::string ConServer::Get_AddressIpClient(const ID_CLIENT idClient) const{
	CONST_HASH_ITERATOR find=this->_mapSockets.find(idClient);
	if(find==this->_mapSockets.end()){
		return std::string("");
	}
	return std::string(SDLNet_ResolveIP(SDLNet_TCP_GetPeerAddress((*find).second)));
}

const bool ConServer::PushData_toClient(const ID_CLIENT idClient, const char* data, const int size_data){
	if(data==NULL){
		this->_errorLOG+="@PushData_toClient: Impossibile effettuare una trasmissione di dati nulli!\n";
		return false;
	}
	BUFFER_DATA& client_data=this->_toSend[idClient];
	Funct::AddMemory_intoVectorChar(data,size_data,client_data);
	return true;
}

const bool ConServer::PushData_toClient(const ID_CLIENT clientID, BUFFER_DATA& buffer){
	Funct::TransfBuffer(this->_toSend[clientID],buffer);
	return true;
}

}





