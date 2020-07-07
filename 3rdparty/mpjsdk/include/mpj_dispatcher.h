//---------------------------------------------------------------------------
// Moving Projector SDK
// (C) Sony Corporation. All rights reserved.
//! \file mpj_dispatcher.h
//! \brief Dispatcher support
//---------------------------------------------------------------------------

#ifndef _mpj_dispatcher_h_
#define _mpj_dispatcher_h_

#include <memory>
#include <functional>

namespace mpjsdk
{
	//! Implements dispatcher.
	class mpj_Dispatcher
	{
	public:
		//! Initialize the instance on the calling thread.
		mpj_Dispatcher();
		//! Release the instance.
		~mpj_Dispatcher();

		//! Action type definition.
		using Action = std::function<void()>;

		//! Post an action.
		/*!
		\param action An action executed asynchronously on the dispatcher's associated thread.
		*/
		void post(const Action& action) const;
		//! Send an action.
		/*!
		\param action An action executed synchronously on the dispatcher's associated thread.
		The call blocks until the execution finishes.
		*/
		void send(const Action& action) const;

	private:
		void* context;
	};
}

#endif // _mpj_dispatcher_h_