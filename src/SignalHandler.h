#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

class SignalHandler {
	public:
		enum EventType {E_TIMER, E_INTERRUPT};
	private:
		bool interrupted;
		void OnEvent(EventType);
		void OnTimer();
		void OnInterrupt();
		long long int ticks;
	public:
		SignalHandler();
		~SignalHandler();
		bool Interrupted() const;
		long long int Ticks() const;
		friend void sig_handler(int);
};

#endif

