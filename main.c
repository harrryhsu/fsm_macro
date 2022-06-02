#include <stdio.h>
#include <stdbool.h>

void Nop() {}

static long dStartTimer = 0;

struct FSM {
    void* label;
    void* end;
} FSM;

#define _INNER_FSM_BEGIN(body, count) {\
    static struct FSM fsm;\
    fsm.end = &&FSM_END_BLOCK_##count;\
    if (fsm.label) goto *fsm.label;\
    body\
        fsm.label = 0;\
    FSM_END_BLOCK_##count:\
        Nop();\
}

#define _AUX_FSM_BEGIN(body, count) _INNER_FSM_BEGIN(body, count)
#define FSM_BEGIN(body) _AUX_FSM_BEGIN(body, __COUNTER__)

#define _FSM_ASYNC_BLOCK(cond, c)\
    FSM_ROUTINE_##c:\
        Nop();\
    fsm.label = && FSM_ROUTINE_##c;\
    if (!cond) goto *fsm.end;

#define _AUX_FSM_WAIT_FOR(cond, c) _FSM_ASYNC_BLOCK(cond, c)
#define FSM_WAIT_FOR(cond) _AUX_FSM_WAIT_FOR(cond, __COUNTER__)

#define _INNER_FSM_DELAY_MS(ms, c) \
    FSM_ROUTINE_##c:\
        Nop();\
    fsm.label = && FSM_ROUTINE_##c;\
    static int cur_##c = -1;\
    if (cur_##c == -1) cur_##c = dStartTimer;\
    if (dStartTimer - cur_##c >= ms) cur_##c = -1;\
    else goto *fsm.end;\

#define _AUX_FSM_DELAY_MS(ms, c) _INNER_FSM_DELAY_MS(ms, c)
#define FSM_DELAY_MS(ms) _AUX_FSM_DELAY_MS(ms, __COUNTER__)

bool flag = false;

void test(){
    FSM_BEGIN({
        FSM_DELAY_MS(2)
        printf("Flag 1\n");
        FSM_WAIT_FOR(flag)
        printf("Flag 2\n");
        flag=false;
    })
    
    FSM_BEGIN({
        FSM_DELAY_MS(10)
        printf("Flag toggled\n");
        flag = true;
    })
}

void main(){
  while(1) {
      dStartTimer+=1;
      printf("Time: %ld\n", dStartTimer);
      test();
      sleep(1);
  }
}