/*
 * ConClient.cpp
 *
 *  Created on: 01/set/2012
 *      Author: neo
 */

#include <XNet/client/ConClient.hpp>
#include <cassert>

namespace XNet {

const Uint16 ConClient::_portdefault = 8990;
const std::string ConClient::_hostnamedefault = "localhost";
const Uint32 ConClient::_delay_checkdefault = 0;
const size_t ConClient::_MAXsizeBytesBufferdefault = 512;

ConClient::ConClient(const std::string& Namehost, const Uint16 portCostruc):_socket(NULL),_setsocket(NULL),
		_delay_check(_delay_checkdefault),_MAXsizeBytesBuffer(_MAXsizeBytesBufferdefault){
	this->Set_Port(portCostruc);
	this->Set_HostName(Namehost);
}

ConClient::~ConClient(){
	this->CloseConnection();
}

const bool ConClient::Set_Port(const Uint16 portSet){
	if(this->_socket!=NULL){
		this->_errorLog+="@Set_Port: Impossibile eseguire una configurazione della connessione mentre quest ultima è aperta\n";
		return false;
	}
	this->_port=portSet;
	return true;
}

const bool ConClient::Set_HostName(const std::string& hostnameSet){
	if(this->_socket!=NULL){
		this->_errorLog+="@Set_Port: Impossibile eseguire una configurazione della connessione mentre quest ultima è aperta\n";
		return false;
	}
	this->_hostname=hostnameSet;
	return true;
}

const bool ConClient::OpenConnection(){
	if(this->_socket!=NULL){
		this->_errorLog+="@OpenConnection: Impossibile aprire una connessione in corso. Chiudere prima quella in esecuzione\n";
		return false;
	}
	if(SDLNet_ResolveHost(&this->_ipaddress, this->_hostname.c_str(), this->_port)!=0){
		this->_errorLog+="@OpenConnection: Impossibile risolvere l'host assegnato!\n";
		this->_errorLog+=SDLNet_GetError();
		this->_errorLog+='\n';
		return false;
	}

	this->_socket = SDLNet_TCP_Open(&this->_ipaddress);
	if(this->_socket==NULL){
		this->_errorLog+="@OpenConnection: Impossibile aprire la connessione\n";
		this->_errorLog+=SDLNet_GetError();
		this->_errorLog+='\n';
		return false;
	}

	this->_setsocket = SDLNet_AllocSocketSet(1);
	if(this->_setsocket==NULL){
		this->_errorLog+="@OpenConnection: Impossibile allocare la memoria-SET necessaira\n";
		this->_errorLog+=SDLNet_GetError();
		this->_errorLog+='\n';
		this->CloseConnection();
		return false;
	}

	SDLNet_TCP_AddSocket(this->_setsocket,this->_socket);

	return true;
}

void ConClient::CloseConnection(){
	if(this->_socket!=NULL){
		if(this->_setsocket!=NULL){
			SDLNet_TCP_DelSocket(this->_setsocket, this->_socket);
			SDLNet_FreeSocketSet(this->_setsocket);
			this->_setsocket=NULL;
		}
		SDLNet_TCP_Close(this->_socket);
		this->_socket=NULL;
		while(this->_dataRcv.empty()==false){
			this->_dataRcv.pop();
		}
		while(this->_dataToSend.empty()==false){
			this->_dataToSend.pop();
		}
	}
}

const int ConClient::UpDateConnection(){
	assert(this->_socket);
	int result=ConClient::NONE;

	size_t bytes_to_send= this->_dataToSend.size();
	if(bytes_to_send > 0){
		if(bytes_to_send > this->_MAXsizeBytesBuffer){
			bytes_to_send = this->_MAXsizeBytesBuffer;
		}
		char buffer_temp_send[bytes_to_send];
		Funct::MakeMemory_fromVectorChar(buffer_temp_send,bytes_to_send,this->_dataToSend);
		if(SDLNet_TCP_Send(this->_socket, buffer_temp_send, bytes_to_send)<(int)(bytes_to_send)){
			this->_errorLog+="@UpDateConnection: Errore in trasmissione del buffer dati.\n";
			this->_errorLog+=SDLNet_GetError();
			this->_errorLog+='\n';
			result=result|WITH_ERRORS;
		}
	}

	int sockets_ready=SDLNet_CheckSockets(this->_setsocket, this->_delay_check);
	if(sockets_ready>0){
		if(SDLNet_SocketReady(this->_socket)!=0){
			char buffer_temp[this->_MAXsizeBytesBuffer];
			int bytes_rcv=SDLNet_TCP_Recv(this->_socket, buffer_temp, this->_MAXsizeBytesBuffer);
			if(bytes_rcv<=0){
				result=result|ConClient::SERVER_CRASH;
				this->CloseConnection();
			}else{
				Funct::AddMemory_intoVectorChar(buffer_temp,bytes_rcv,this->_dataRcv);
				result=result|ConClient::STREAM_RCV;
			}
		}
	}
	return result;
}

const bool ConClient::PushData_toServer(const char* data, const int size_data) {
	if(data==NULL){
		this->_errorLog+="@PushData_toServer: Impossibile inviare i dati NULLI\n";
		return false;
	}
	Funct::AddMemory_intoVectorChar(data,size_data,this->_dataToSend);
	return true;
}

const bool ConClient::PushData_toServer(BUFFER_DATA& data_queue) {
	Funct::TransfBuffer(this->_dataToSend,data_queue);
	return true;
}


}








 /* namespace XNet */
