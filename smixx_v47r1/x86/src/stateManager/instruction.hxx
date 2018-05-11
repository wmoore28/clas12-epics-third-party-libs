
#ifndef INSTRUCTION_HH
#define INSTRUCTION_HH

class Name;

class Instruction {
public:
    virtual ~Instruction() { return;}
    virtual void whatAreYou()  = 0;
    virtual int execute(Name& endState)  = 0;   // 0-normal,  1-terminated action, 2-suspended
};  

#endif
