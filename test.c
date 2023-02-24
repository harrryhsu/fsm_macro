#include "fsm.c"

bool flag = false;
FSM *fsmp;

void test()
{
	FSM_BEGIN(fsmp, {
		FSM_DELAY_MS(5);
		flag = true;
		FSM_WAIT_FOR(false);
	});

	FSM_BEGIN({
		FSM_DELAY_MS(1);
		printf("Flag 1\n");
		FSM_WAIT_FOR(FSM_DELAY_MS_RET(1))
		printf("Flag 2\n");
		FSM_WAIT_FOR(flag)
		printf("Flag 3\n");
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