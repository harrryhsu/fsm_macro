# FSM

FSM is a pure c macro implementation of rabbit dynamic c's costatement function

## Usage

```c
void interrupt_on_1ms() {
 StartTimeInMs++;
}

while(1){
 FSM_BEGIN({
  FSM_DELAY_MS(3);
  printf("Flag 1\n");
  FSM_WAIT_FOR(boolean)
  printf("Flag 2\n");
  FSM_WAIT_FOR(FSM_DELAY_MS_RET(3))
  printf("Flag 3\n");
 });
}
```

## License

[MIT](https://choosealicense.com/licenses/mit/)
