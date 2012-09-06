/*
 * ConServer.hpp
 *
 *  Created on: 31/ago/2012
 *      Author: neo
 */

#ifndef CONSERVER_HPP_
#define CONSERVER_HPP_

#include <string>
#include <SDL/SDL_net.h>
#include <XNet/core/lib_set.hpp>
#include <XNet/core/Funct.hpp>
#include <boost/utility.hpp>
#include <map>
#include <vector>

namespace XNet {

typedef int ID_CLIENT;
typedef std::vector<ID_CLIENT> LISTA_CLIENT;


class XNET_DLL ConServer: private boost::noncopyable{
public:
	/*Cotruttore. Instanza un oggetto 'connessione server'*/
	ConServer(const Uint16 port =8990);

	/*Distruttore*/
	~ConServer();

	/*Get_LogError
	 * 		Ritorna la stringa contenente il log degli errore verificatosi durante l'elaborazione delle funzioni membro
	 * 		di questa instanza di connessione*/
	const std::string& Get_LogError(void) const{
		return this->_errorLOG;
	}

	/*Set_PortConnection
	 * Setta il porto della connessione.
	 * IMPORTANTE: utilizzare questo metodo quando la connessione non è stata ancora avviata,
	 * in caso contrario è necessario terminare la connessione in corso e riavviarla*/
	const bool Set_PortConnection(const Uint16);

	/*Get_PortConnection
	 * Ritorna il porto della connessione in corso*/
	const Uint32 Get_PortConnection(void) const{
		return this->_porto_connessione;
	}

	/*Set_MaxUsers
	 * Setta il massimo numero di utenza collegabili per questa connessione server.
	 * Una volta raggiunto il massimo, ogni connessione in arrivo verrà respinta inviando un messaggio
	 * che specificherà a tale client che il server è pieno
	 * NOTA BENE: questo parametro deve essere settato prima di aprire la connessione server, altrimenti non avrà
	 * alcun effetto (e ritornerà un errore)
	 * Ritorna 'true' se la modifica ha effetto positivo sulla configurazione della connessione*/
	const bool Set_MaxUsers(const unsigned int);

	/*Get_MaxUsers
	 * Ritorna il numero di utenti massimi definiti per questa connessione server*/
	const unsigned int Get_MaxUsers(void) const{
		return this->_clientsMAX;
	}

	/*Get_UsersInServer
	 * 		Ritorna il numero di connessione attualmente presenti su l'instanza di questa connessione server
	 * 		(il numero dei client connessi)*/
	const size_t Get_UsersInServer(void) const{
		return this->_mapSockets.size();
	}

	/*Get_AddressIpClient
	 * 		Ritorna la stringa rappresentate (in decimale) l'indirizzo IP del client identificato*/
	const std::string Get_AddressIpClient(const ID_CLIENT) const;

	/*Get_AddressIpServer
	 * 		Ritorna la stringa rappresentate (in decimale) l'indirizzo IP del server instanziato*/
	const std::string Get_AddressIpServer(void) const;

	/*Set_MaxBytesBuffer
	 * 		Setta la dimensione del buffer di ricezione dei messaggi.
	 * 		Questa dimensione (espressa in bytes) indica la dimensione massima dei messaggi che possono essere ricevuti.
	 * 		Oltre questa dimensione i messaggi verranno elaborati in 2 cicli di UPDATE
	 * 		NOTA BENE: questa funzione deve essere chiamata prima di aprire la connessione, altrimenti ritornetà
	 * 		'false' e non avrà effetto*/
	const bool Set_MaxBytesBuffer(const int);

	/*Get_MaxBytesBuffer
	 * 		Ritorna il numero di bytes destinati al buffer dei messaggi in ricezione.
	 * 		Messaggi che hanno una dimensione maggiore di questo buffer verranno elaborati in 2 cicli*/
	const int Get_MaxBytesBuffer(void) const{
		return this->_MAXbufferSizeMessage;
	}

	/*OpenConnection
	 * Apre la connessione attraverso la porta definita in precedenza (vedi costruttore o funzione membro apposita)
	 * Non è possibile chiamare questa funzione se è già in corso una connessione. In tal caso è necessario chiudere la connessione
	 * in corso e riavvare
	 * Ritorna 'true' se la connessione viene aperta correttamente*/
	const bool OpenConnection();

	/*CloseConnection
	 * Chiude la connessione in corso, se n'è presente una.*/
	void CloseConnection();

	/*CloseAConnectionClient
		 * Chiude la connessione con un client identificato dal parametro.
		 * Ritorna 'true' se il client viene correttamente chiuso e la connessione terminata, altrimenti
		 * 'false' nel caso il client non esista*/
	const bool CloseAConnectionClient(const ID_CLIENT);

	/*UpDateConnection
	 * 		Aggiorna la connessione del server.
	 * 			FASI:	1) Controlla se ci sono connessioni in arrivo e le aggiunge;
	 * 					2) Controlla se i client hanno inviato messaggi e li memorizza in una mappa
	 * 					3) Eventualmente disconnette i client che danno errore o crashano
	 * 		E' possibile sapere quali fasi vengono effettuate attraverso la maschera con le costanti enumerate
	 * 		(vedi RESULT_UPDATE)
	 * 				ESEMPIO:
	 * 					if(UpDateConnection()&NEW_CLIENT){
	 * 						C'è stata una nuova connessione!
	 * 					}
	 *
	 * 		NOTA BENE: E' possibile chiamare questa funzione solo dopo aver aperto la connessione, altrimenti si rischia
	 * 		una crash dell'applicazione!
	 *
	 * 		PARAMETRI:
	 * 				void (*)(const ID_CLIENT)		=		(input)	Puntatore a funzione che verrà eseguito all'atto della
	 * 																registrazione di un nuovo client, il parametro della funzione
	 * 																è l'id della nuova connessione in arrivo
	 * 				void (*)(const ID_CLIENT)		=		(input)	Puntatore a funzione che verrà eseguito all'atto della
	 * 																disconnessione di un client.
	 * 				void (*)(const ID_CLIENT)		=		(input)	Puntatore a funzione che verrà eseguito se un client
	 * 																trasmette uno stream di dati. I parametri della funzione
	 * 																saranno l'ID del client e un indirizzo di memoria contenente la
	 * 																copia dei dati trasmetti (TEMPORANEI) più la dimensione della
	 * 																memoria*/
	const int UpDateConnection(void(*)(const ID_CLIENT)=NULL, void(*)(const ID_CLIENT)=NULL,
			void(*)(const ID_CLIENT, const char*, const int)=NULL);
	enum RESULT_UPDATE{
		NONE					=	0x0,
		NEW_CLIENT				=	0x1,
		CLIENT_DISCONNECT 		=	0x2,
		CLIENT_RCVDATA			=	0x4,
		WITH_ERROR				=	0x8,
		SEND_DATA				=	0x10
	};

	/*Get_ListaNuoviClient
	 * 		Ritorna una lista (vettore) di client (identificati dal loro ID univoco) che si sono collegati dopo
	 * 		la chiamata della funzione 'UpDateConnection'
	 * 		NOTA BENE: La chiamata della funzione 'UpDateConnection' aggiorna questo vettore*/
	XNET_INLINE const LISTA_CLIENT& Get_ListaNuoviClient() const{
		return _listClientsLog;
	}

	/*Get_ListaDisconnectClient
	 * Ritorna una lista (vettore) di client (identificati dal loro ID univoco) che si sono disconnessi dopo
	 * 		la chiamata della funzione 'UpDateConnection'
	 * 		NOTA BENE: La chiamata della funzione 'UpDateConnection' aggiorna questo vettore*/
	XNET_INLINE const LISTA_CLIENT& Get_ListaDisconnectClient() const{
		return _listClientsBye;
	}

	/*Get_ListaDataClient
	 * Ritorna una lista (vettore) di client (identificati dal loro ID univoco) che hanno trasmesso uno stream di dati dopo
	 * 		la chiamata della funzione 'UpDateConnection'
	 * 		NOTA BENE: La chiamata della funzione 'UpDateConnection' aggiorna questo vettore*/
	XNET_INLINE const LISTA_CLIENT& Get_ListaDataClient() const{
		return _listClientsProc;
	}

	/*PushData_toClient
	 * 		Questa funzione inserisce uno stream di dati che verrà messo in coda per essere processato.
	 * 		L'invio, dunque, non è instantaneo e produce leggermente overhead .. risultà, però, un metodo più sicuro
	 * 		per l'invio dei dati ai client.
	 * 		L'invio definitivo di TUTTI i messaggi-dati in coda verrà effettuato all'atto della chiamata al metodo
	 * 		'UpDateConnection'
	 *
	 * 		PARAMETRI:
	 * 			const ID_CLIENT			=		(input)	L'identificativo univoco che è assegnato al client al quale
	 * 													inviare lo stream di dati
	 * 			const char*				=		(input)	Un puntatore ad un area di memoria; Quest area di memoria
	 * 													verrà inviata al client
	 * 			const int				=		(input)	La dimensione (in bytes) dello stream di dati
	 *
	 * 		VALORE DI RITORNO:
	 * 			La funzione ritorna 'true' se l'operazione va a buon fine, altrimenti 'false'
	 *
	 * 		NOTA BENE: l'invio è condizionato dalla dimensione del buffer settata nella configurazione della connessione.
	 * 			Se i dati immessi superano quelli della dimensione del buffer l'invio verrà effettuato in più cicli fino
	 * 			all'esaurimento di tutto i dati nel buffer*/
	const bool PushData_toClient(const ID_CLIENT, const char*, const int);

	/*PushData_toClient
	 * 		Overloading. Vedi info precedente...
	 *
	 * 		NOTA BENE: il BUFFER_DATA in ingresso verrà svuotato completamente*/
	const bool PushData_toClient(const ID_CLIENT, BUFFER_DATA&);

	/*Get_QueueRcvToClient
	 * 		Riceve il buffer-data-stream ricevuto dal client identificato dal parametro.
	 * 		NOTA BENE: E' compito del chiamante decodificare i dati, e cancellare quelli non più necessari.
	 *
	 * 		PARAMETRI:
	 * 			const ID_CLIENT				=			(input)	Identificativo univoco del client di cui si vuole leggere i dati
	 * 															trasmessi
	 *
	 * 		VALORE DI RITORNO:
	 * 			La funzione ritorna il riferimento allo stream dei dati trasmetti dal client*/
	XNET_INLINE BUFFER_DATA& Get_QueueRcvToClient(const ID_CLIENT idClient){
		return this->_mapDataClients[idClient];
	}









protected:		//DATA
	typedef std::map<ID_CLIENT, TCPsocket> HASH_MAP;
	typedef HASH_MAP::iterator HASH_ITERATOR;
	typedef HASH_MAP::const_iterator CONST_HASH_ITERATOR;
	typedef std::map<ID_CLIENT, BUFFER_DATA> DATA_CLIENTS;

	Uint16 _porto_connessione;
	std::string _errorLOG;
	TCPsocket _serversocket;
	IPaddress _ipaddress;
	unsigned int _clientsMAX;
	SDLNet_SocketSet _socketset;
	unsigned int _IDmemCount;
	HASH_MAP _mapSockets;
	Uint32 _delayCheckSockets;
	int _MAXbufferSizeMessage;
	DATA_CLIENTS _mapDataClients;
	LISTA_CLIENT _listClientsLog;
	LISTA_CLIENT _listClientsBye;
	LISTA_CLIENT _listClientsProc;
	DATA_CLIENTS _toSend;

	static const unsigned int _clientsMAXdefault;
	static const Uint32 _delayCheckSocketsdefault;
	static const int _MAXbufferSizeMessagedefault;

private:
	/*PRIVATE: RegisterIncomingSocket
	 * Registra una connessione che sta tentando di connettersi assegnadole un IDENTIFICATIVO UNIVOCO
	 * e controllando che il numero di connessioni massime venga rispettato.
	 * Ritorna l'esito della registrazione*/
	enum REG_RTS_VALUE{
		REGISTERED =0,
		SERVER_FULL,
		ERROR
	};
	const REG_RTS_VALUE RegisterIncomingSocket(TCPsocket const, ID_CLIENT&);
};

} /* namespace XNet */
#endif /* CONSERVER_HPP_ */
