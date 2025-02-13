#pragma once
#include <vector>
#include <iostream>
#include "pool.hpp"


class QuestionGame {
	private :
		void  initializeLevel1();
		void  initializeLevel2();
		void  initializeLevel3();
		void  initializeLevel4();
	public :
		struct Question {
			std::string problem;
			char correct;
		};
		std::vector<std::vector<Question> >levels;
		QuestionGame();
		Question & getRandomQuestion(int level);
		std::string  gameMessage(int flag);
		double	logtime(time_t start);

};