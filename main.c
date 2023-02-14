#include <stdio.h>
#include <stdbool.h>

void Nop() {}

static long StartTimeInMs = 0;

typedef struct FSM
{
	void *label;
	void *end;
	int timer[20];
	int timerIndex;
} FSM;

static int ii = 0;

#define FSM_RESET_PROGRESS(fsmp) \
	if (fsmp)                      \
		fsmp->label = 0;
#define FSM_RESET_TIMER(fsmp)   \
	if (fsmp)                     \
		for (ii = 0; ii < 20; ii++) \
			fsmp->timer[ii] = -1;
#define FSM_RESET(fsmp) FSM_RESET_PROGRESS(fsmp) FSM_RESET_TIMER(fsmp)

#define _INNER_FSM_BEGIN(body, c)  \
	{                                \
		static FSM fsm;                \
		fsm.end = &&FSM_END_BLOCK_##c; \
		if (fsm.label)                 \
			goto *fsm.label;             \
		FSM_RESET_TIMER((&fsm));       \
		body;                          \
		fsm.label = 0;                 \
		FSM_END_BLOCK_##c : Nop();     \
	}

#define _AUX_FSM_BEGIN(body, c) _INNER_FSM_BEGIN(body, c)
#define __AUX_FSM_BEGIN(body) _AUX_FSM_BEGIN(body, __COUNTER__)

#define _INNER_FSM_BEGIN_EXT(fsmp, body, c) \
	{                                         \
		static FSM fsm;                         \
		fsmp = &fsm;                            \
		fsm.end = &&FSM_END_BLOCK_##c;          \
		if (fsm.label)                          \
			goto *fsm.label;                      \
		FSM_RESET_TIMER((&fsm));                \
		body;                                   \
		fsm.label = 0;                          \
		FSM_END_BLOCK_##c : Nop();              \
	}

#define _AUX_FSM_BEGIN_EXT(fsmp, body, c) _INNER_FSM_BEGIN_EXT(fsmp, body, c)
#define __AUX_FSM_BEGIN_EXT(fsmp, body) _AUX_FSM_BEGIN_EXT(fsmp, body, __COUNTER__)

#define FSM_GET_MACRO(_1, _2, NAME, ...) NAME
#define FSM_BEGIN(...) FSM_GET_MACRO(__VA_ARGS__, __AUX_FSM_BEGIN_EXT, __AUX_FSM_BEGIN)(__VA_ARGS__)

#define _FSM_ASYNC_BLOCK(cond, c) \
	FSM_ROUTINE_##c : Nop();        \
	fsm.label = &&FSM_ROUTINE_##c;  \
	if (!cond)                      \
		goto *fsm.end;

#define _AUX_FSM_WAIT_FOR(cond, c) _FSM_ASYNC_BLOCK(cond, c)
#define FSM_WAIT_FOR(cond) _AUX_FSM_WAIT_FOR(cond, __COUNTER__)

#define _INNER_FSM_DELAY_MS(ms, c) ({                   \
	static int timer_index_##c = -1;                      \
	if (timer_index_##c == -1)                            \
		timer_index_##c = fsm.timerIndex++;                 \
	if (fsm.timer[timer_index_##c] == -1)                 \
		fsm.timer[timer_index_##c] = StartTimeInMs;         \
                                                        \
	int __ret = 0;                                        \
	if (StartTimeInMs - fsm.timer[timer_index_##c] >= ms) \
	{                                                     \
		fsm.timer[timer_index_##c] = -1;                    \
		__ret = 1;                                          \
	}                                                     \
	__ret;                                                \
})

#define _AUX_FSM_DELAY_MS(ms, c) _INNER_FSM_DELAY_MS(ms, c)
#define FSM_DELAY_MS(ms) _AUX_FSM_DELAY_MS(ms, __COUNTER__)

bool flag = false;
FSM *fsmp;

void test()
{
	FSM_BEGIN(fsmp, {
		FSM_WAIT_FOR(FSM_DELAY_MS(5))
		flag = true;
	});

	FSM_BEGIN({
		FSM_WAIT_FOR(FSM_DELAY_MS(2))
		printf("Flag 1\n");
		FSM_WAIT_FOR(flag)
		printf("Flag 2\n");
		flag = false;
		FSM_RESET(fsmp);
	});
}

void main()
{
	while (1)
	{
		StartTimeInMs += 1;
		printf("Time: %ld\n", StartTimeInMs);
		test();
		sleep(1);
	}
}