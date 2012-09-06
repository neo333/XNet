/*
 * Event.hpp
 *
 *  Created on: 01/set/2012
 *      Author: neo
 */

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <XNet/core/lib_set.hpp>

#ifndef NULL
#define NULL 0
#endif

namespace XNet {

template<class DATA> XNET_DLL class Event{
public:
	/*Costruttore*/
	Event(void (*funzione)(DATA*) =NULL, DATA* operand =NULL):funz_ctrl(funzione),param(operand){

	}

	/*Execute
	 * 		Esegue l'evento.*/
	void Execute(DATA* operand =NULL){
		if(this->funz_ctrl){
			if(operand!=NULL){
				this->funz_ctrl(operand);
			}else{
				this->funz_ctrl(this->param);
			}
		}
	}

private:
	void (*funz_ctrl)(DATA*);
	DATA* param;
};

} /* namespace XNet */
#endif /* EVENT_HPP_ */
