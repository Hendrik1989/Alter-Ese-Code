#ifndef SRC_MAIN_LOGIC_BZ_BZAUSLAUF_H_
#define SRC_MAIN_LOGIC_BZ_BZAUSLAUF_H_


#include "../Basestate.h"
#include "../BZ/BZ.h"



class BZAuslauf: public Basestate {
public:

    void entry() override;
    void exit() override;
    void estp() override;
    void doAction() override;
    //void showState() override;
};

#endif /* SRC_MAIN_LOGIC_BZ_BZAUSLAUF_H_ */
