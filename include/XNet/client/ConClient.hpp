/*
 * ConClient.hpp
 *
 *  Created on: 01/set/2012
 *      Author: neo
 */

#ifndef CONCLIENT_HPP_
#define CONCLIENT_HPP_

#include <XNet/core/lib_set.hpp>
#include <XNet/core/Event.hpp>
#include <XNet/core/Funct.hpp>
#include <boost/utility.hpp>
#include <string>
#include <SDL/SDL_net.h>

namespace XNet{

class XNET_DLL ConClient: private boost::noncopyable{
public:
	/*Costruttore*/
	ConClient(const std::string& =_hostnamedefault,const Uint16 =_portdefault);

	/*Distruttore*/
	~ConClient();

	/*Set_Port
	 * 		Setta la porta per la configurazione della connessione al server.
	 * 		Ritorna 'true' se la nuova configurazione viene accettata.
	 * 		NOTA BENE: Non è possibile richiamare questa funziona dopo aver aperto la connessione, in caso
	 * 			è necessario riavviare la connessione stessa*/
	const bool Set_Port(const Uint16);

	/*Get_Port
	 * 		Ritorna la porta attuale della configurazione della connessione al server.*/
	const Uint16 Get_Port() const{
		return this->_port;
	}

	/*Set_HostName
	 * 		Setta il nome dell'host al quale effettuare la connessione.
	 * 		Ritorna 'true' se la nuova configurazione viene accettata.
	 * 		NOTA BENE: Non è possibile richiamare questa funziona dopo aver aperto la connessione, in caso
	 * 			è necessario riavviare la connessione stessa*/
	const bool Set_HostName(const std::string&);

	/*Get_HostName
	 * 		Ritorna il nome dell'host attualmente configurato per la connessione.*/
	const std::string& Get_HostName() const{
		return this->_hostname;
	}

	/*Get_LogError
	 * 		Ritorna il log degli errori verificatosi all'interno della connessione*/
	const std::string& Get_LogError() const{
		return this->_errorLog;
	}

	/*OpenConnection
	 * 		Apre la connessione al server.
	 * 		E' necessario configurare la connessione tramite gli appositi metodi della classe (ad esempio porta e hostname)
	 * 		Ritorna 'true' se la connessione viene stabilita, altrimenti false*/
	const bool OpenConnection();

	/*CloseConnection
	 * 		Chiude la connessione al server in corso.*/
	void CloseConnection();

	/*UpDateConnection
	 * 		Aggiorna la connessione al server. Verifica se il server è disconnesso (in caso termina la connessione)
	 * 		E aggiorna lo stream in ricezione.
	 * 		Ritorna un insieme di valori che identificano gli eventi processati nella funzione.
	 * 		Per controllare un evento è possibile effettuare una maschera sul valore di ritorno
	 * 			Esempio:
	 * 				if(UpDateConnection()&WITH_ERRORS) do it;*/
	const int UpDateConnection();
	enum RESULT_UPDATE_CON{
		NONE			=		0x0,
		STREAM_RCV		=		0x1,
		SERVER_CRASH	=		0x2,
		WITH_ERRORS		=		0x4
	};

	/*Get_QueueDataRcv
	 * 		Ritorna un riferimento alla coda (stream di dati) di ricezione dal server.
	 * 		Il chiamate ha la responsabilità di interpretare eventuali dati ed eseguire il POP (cancellazione dei dati letti).*/
	XNET_INLINE BUFFER_DATA& Get_QueueDataRcv(){
		return this->_dataRcv;
	}

	/*PushData_toServer
		 * 		Questa funzione inserisce uno stream di dati che verrà messo in coda per essere processato.
		 * 		L'invio, dunque, non è instantaneo e produce leggermente overhead .. risultà, però, un metodo più sicuro
		 * 		per l'invio dei dati al server.
		 * 		L'invio definitivo di TUTTI i messaggi-dati in coda verrà effettuato all'atto della chiamata al metodo
		 * 		'UpDateConnection'
		 *
		 * 		PARAMETRI:
		 * 			const char*				=		(input)	Un puntatore ad un area di memoria; Quest area di memoria
		 * 													verrà inviata al client
		 * 			const int				=		(input)	La dimensione (in bytes) dello stream di dati
		 *
		 * 		VALORE DI RITORNO:
		 * 			La funzione ritorna 'true' se l'operazione va a buon fine, altrimenti 'false'*/
	const bool PushData_toServer(const char*, const int);

	/*PushData_toServer
	 * 		Overloading. Vedi info precedente ...
	 *
	 * 		NOTA BENE: il buffer in ingresso verrà svuotato*/
	const bool PushData_toServer(BUFFER_DATA&);


protected:
	TCPsocket _socket;
	IPaddress _ipaddress;
	Uint16 _port;
	std::string _hostname;
	std::string _errorLog;
	SDLNet_SocketSet _setsocket;
	Uint32 _delay_check;
	size_t _MAXsizeBytesBuffer;
	BUFFER_DATA _dataRcv;
	BUFFER_DATA _dataToSend;

	static const Uint16 _portdefault;
	static const std::string _hostnamedefault;
	static const Uint32 _delay_checkdefault;
	static const size_t _MAXsizeBytesBufferdefault;
};


} /* namespace XNet */
#endif /* CONCLIENT_HPP_ */
