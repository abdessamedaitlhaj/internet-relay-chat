#include "question.hpp"


QuestionGame::QuestionGame() :  started(false), questionSent(false), correctAnswer(' '), level(1)
{
		initializeLevel1();
		initializeLevel2();
		initializeLevel3();
		initializeLevel4();
}

void QuestionGame::initializeLevel1() {
            std::vector<Question> level1;
            level1.push_back(Question());
            level1[0].problem = Q11;
            level1[0].correct = 'c';
            
            level1.push_back(Question());
            level1[1].problem = Q12;
            level1[1].correct = 'b';

            level1.push_back(Question());
            level1[2].problem = Q13;
            level1[2].correct = 'b';

            level1.push_back(Question());
            level1[3].problem = Q14;
            level1[3].correct = 'a';

            level1.push_back(Question());
            level1[4].problem = Q15;
            level1[4].correct = 'b';

            levels.push_back(level1);
}

void QuestionGame::initializeLevel2() {
	        std::vector<Question> level2;
            level2.push_back(Question());
            level2[0].problem = Q21;
            level2[0].correct = 'a';
            
            level2.push_back(Question());
            level2[1].problem = Q22;
            level2[1].correct = 'b';

            level2.push_back(Question());
            level2[2].problem = Q23;
            level2[2].correct = 'c';

            level2.push_back(Question());
            level2[3].problem = Q24;
            level2[3].correct = 'b';

            level2.push_back(Question());
            level2[4].problem = Q25;
            level2[4].correct = 'a';

            levels.push_back(level2);
}

void QuestionGame::initializeLevel3() {

	    std::vector<Question> level3;
        level3.push_back(Question());
        level3[0].problem = Q31;
        level3[0].correct = 'c';

		level3.push_back(Question());
        level3[1].problem = Q32;
        level3[1].correct = 'a';

		level3.push_back(Question());
        level3[2].problem = Q33;
        level3[2].correct = 'c';

        level3.push_back(Question());
        level3[3].problem = Q34;
        level3[3].correct = 'a';

    	level3.push_back(Question());
        level3[4].problem = Q35;
        level3[4].correct = 'a';

        levels.push_back(level3);
}

void QuestionGame::initializeLevel4() {

		std::vector<Question> level4;
        level4.push_back(Question());
        level4[0].problem = Q41;
        level4[0].correct = 'a';

		level4.push_back(Question());
        level4[1].problem = Q42;
        level4[1].correct = 'a';

		level4.push_back(Question());
        level4[2].problem = Q43;
        level4[2].correct = 'a';

        level4.push_back(Question());
        level4[3].problem = Q44;
        level4[3].correct = 'b';

    	level4.push_back(Question());
        level4[4].problem = Q45;
        level4[4].correct = 'b';

        levels.push_back(level4);



}


QuestionGame::Question &QuestionGame::getRandomQuestion(int level) {
    srand(time(0));
    int index = rand() % levels[level - 1].size();
    return levels[level - 1][index];
} 

std::string QuestionGame::gameMessage(int flag)
{
    if (flag == MENU)
        return MENU_MESSAGE;
    
    if (flag == END)
        return END_MESSAGE;
    
    if (flag == WELCOME)
        return WELCOME_MESSAGE;
    
    if (flag == LVL1)
        return LVL1_MESSAGE;
    
    if (flag == LVL2)
        return LVL2_MESSAGE;
    
    if (flag == LVL3)
        return LVL3_MESSAGE;
    
    if (flag == LVL4)
        return LVL4_MESSAGE;
    
	return (END_MESSAGE);
}


int QuestionGame::getLevel() const {
    return level;
}

bool QuestionGame::getStarted() const {
    return started;
}

bool QuestionGame::isQuestionSent() const {
    return questionSent;
}

char QuestionGame::getAnswer() const {
    return correctAnswer;
}

void QuestionGame::setLevel(int newLevel) {
    level = newLevel;
}

void QuestionGame::setStarted(bool startedFlag) {
    started = startedFlag;
}

void QuestionGame::setQuestionSent(bool flag) {
    questionSent = flag;
}

void QuestionGame::setAnswer(char answer) {
    correctAnswer = answer;
}

void QuestionGame::addLevel() {
    level++;
}

