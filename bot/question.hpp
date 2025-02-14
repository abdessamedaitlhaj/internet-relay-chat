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
		int level;
    	bool started;
    	bool questionSent;
    	char correctAnswer; 
	public :
		struct Question {
			std::string problem;
			char correct;
		};
		std::vector<std::vector<Question> >levels;
		QuestionGame();
		Question & getRandomQuestion(int level);
		std::string  gameMessage(int flag);
		int getLevel() const { return level; }
    	bool getStarted() const { return started; }
    	bool isQuestionSent() const { return questionSent; }
    	char getAnswer() const { return correctAnswer; }

    	void setLevel(int newLevel) { level = newLevel; }
    	void setStarted(bool startedFlag) { started = startedFlag; }
    	void setQuestionSent(bool flag) { questionSent = flag; }
    	void setAnswer(char answer) { correctAnswer = answer; }
    	void addLevel() {
			level++; 
			if (level == 5)
				level = 1;
		}

};