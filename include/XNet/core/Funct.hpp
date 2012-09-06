/*
 * Funct.hpp
 *
 *  Created on: 01/set/2012
 *      Author: neo
 */

#ifndef FUNCT_HPP_
#define FUNCT_HPP_

#include <XNet/core/lib_set.hpp>
#include <boost/utility.hpp>
#include <queue>
#include <list>

namespace XNet {

typedef std::queue<char, std::list<char> > BUFFER_DATA;

class XNET_DLL Funct: private boost::noncopyable{
public:
	/*AddMemory_intoVectorChar
	 * 		AGGIUNGE in coda (Back()) un'area di memoria definita da un puntatore e una dimesione ad un vettore di
	 * 		caratteri*/
	XNET_INLINE static void AddMemory_intoVectorChar(const char* data, const int size_data, BUFFER_DATA& vector_char){
		for(int i=0; i<size_data; i++){
			vector_char.push(data[i]);
		}
	}

	/*MakeMemory_fromVectorChar
	 * 		Riempie un'area di memoria di tutto il contenuto del BUFFER_DATA.
	 *
	 *		PARAMETRI:
	 *				char* 			=				(output)Rappresenta l'indirizzo di memoria dove verranno trasferiti i dati.
	 *				const size_t	=				(input)	E' la dimensione del vettore 'data'. Sono i bytes totali
	 *														che verranno trasferiti dal BUFFER_DATA alla memoria.
	 *				BUFFER_DATA&	=				(input)	E' il BUFFER_DATA da dove verranno prelevati (e anche eliminati)
	 *														i dati letti.
	 *
	 * 		NOTA BENE: il BUFFERDATA (vettore stream) in input verrà svuotato!
	 * 		NOTA BENE: assicurarsi di aver allocato 'data' di una grandezza pari a 'size_data'!*/
	XNET_INLINE static void MakeMemory_fromVectorChar(char* data, const size_t size_data, BUFFER_DATA& vector_char){
		size_t i=0;
		while(vector_char.empty()==false && i<size_data){
			data[i]=vector_char.front();
			vector_char.pop();
		}
	}

	/*TransfBuffer
	 * 		Trasferisce il contenuto di un buffer nell'altro*/
	XNET_INLINE static void TransfBuffer(BUFFER_DATA& dest, BUFFER_DATA& src){
		while(src.empty()==false){
			dest.push(src.front());
			src.pop();
		}
	}

	/*GetAndPop_toBuffer
	 * 		Ritorna il primo carattere da processare di un buffer-stream, eseguendo anche una pop.
	 * 		La funzione ritorna 'true' se il buffer non è vuoto e l'output è stato salvato correttamente nel riferimento
	 * 		del parametro. Altrimenti ritornerà 'false' e l'output non sarà valido*/
	XNET_INLINE static const bool GetAndPop_toBuffer(BUFFER_DATA& data, char& output){
		if(data.empty()==true) return false;
		output=data.front();
		data.pop();
		return true;
	}

private:
	Funct();
};

} /* namespace XNet */
#endif /* FUNCT_HPP_ */
