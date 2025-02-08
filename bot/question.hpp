#pragma once

#include <vector>
#include <iostream>
class QuestionGame {
	private :
		struct Question {
			std::string problem;
			char correct;
		};
		void  initializeLevel1();
		void  initializeLevel2();
		void  initializeLevel3();
		void  initializeLevel4();
	public :
		std::vector<std::vector<Question> >levels;
		QuestionGame();
		Question & getRandomQuestion(int level);
};