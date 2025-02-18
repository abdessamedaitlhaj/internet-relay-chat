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
    	bool started;
    	bool questionSent;
    	char correctAnswer; 
		int level;
	public :
		struct Question {
			std::string problem;
			char correct;
		};
		std::vector<std::vector<Question> >levels;
		QuestionGame();
		Question & getRandomQuestion(int level);
		std::string  gameMessage(int flag);
		int getLevel() const ;
    	bool getStarted() const ;
    	bool isQuestionSent() const ;
    	char getAnswer() const ;

    	void setLevel(int newLevel);
    	void setStarted(bool startedFlag) ;
    	void setQuestionSent(bool flag) ;
    	void setAnswer(char answer) ;
    	void addLevel() ;
};