
/*******************************************************************
 * @brief	Button click detect:
 * 			SW1. keep press button1 5s === factory reset
 * 			SW1. short press button1   === start device / toggle local light (LED3)
 * 			SW2. keep press button2 5s === invoke EZ-Mode
 * 			SW2. short press button2   === set local permit joining (toggle)
 *
 */
static u32	preampleTick1 = 0;
static u32	preampleTick2 = 0;

typedef enum{
	BT_IDLE,
	BT1_FRISTPRESSED,
	BT1_KEEPPRESSED,
	BT1_RELASED,
	BT2_FRISTPRESSED,
	BT2_KEEPPRESSED,
	BT2_RELASED
}button_st;

typedef enum{
	BUTTON1_CLICKED = BIT(0),
	BUTTON2_CLICKED = BIT(1)
}button_clk_t;

static u8 buttonState = 0;

static u16 getButtonSt(void)
{
	u8 buttonPrevState = buttonState;
	buttonState = (((gpio_read(BUTTON1) ? 0 : 1) << 0)|((gpio_read(BUTTON2) ? 0 : 1) << 1));
	u16 ret = 0;

	u8 bt1On = buttonState & BUTTON1_CLICKED;
	u8 bt1PrevOn = buttonPrevState & BUTTON1_CLICKED;
	if(bt1On && !bt1PrevOn){
		sleep_us(1000*30);
		if((gpio_read(BUTTON1) ? 0 : 1)){
			ret = BT1_FRISTPRESSED;
		}else{
			buttonState &= (~BUTTON1_CLICKED);
		}
	}else if(bt1On && bt1PrevOn){
		ret = BT1_KEEPPRESSED;
	}else if(!bt1On && bt1PrevOn){
		ret = BT1_RELASED;
	}

	u8 bt2On = buttonState & BUTTON2_CLICKED;
	u8 bt2PrevOn = buttonPrevState & BUTTON2_CLICKED;
	if(bt2On && !bt2PrevOn){
		sleep_us(1000*30);
		if((gpio_read(BUTTON2) ? 0 : 1)){
			ret |= (BT2_FRISTPRESSED<<8);
		}else{
			buttonState &= (~BUTTON2_CLICKED);
		}
	}else if(bt2On && bt2PrevOn){
		ret |= (BT2_KEEPPRESSED << 8);
	}else if(!bt2On && bt2PrevOn){
		ret |= (BT2_RELASED << 8);
	}

	return ret;
}

void buttonKeepPressed(u8 btNum){
	if(btNum == SW1){
		zb_factoryReset();
	}else if(btNum == SW2){

	}
}

void preampleTestInterface(void)
{
	u16 ret = getButtonSt();
	button_st st1 = (button_st )(ret & 0xff);
	button_st st2 = (button_st )(ret >> 8);
	if(st1 == BT1_FRISTPRESSED){//button1 first be clicked
		preampleTick1 = clock_time();
	}else if(st1 == BT1_RELASED){
		if(clock_time_exceed(preampleTick1, 3000*1000)){
			preampleTick1 = 0;
			buttonKeepPressed(SW1);
		}else{
			buttonShortPressed(SW1);
		}
	}

	if(st2 == BT2_FRISTPRESSED){
		preampleTick2 = clock_time();
	}else if(st2 == BT2_RELASED){
		if(clock_time_exceed(preampleTick2, 3000*1000)){
			preampleTick2 = 0;
			buttonKeepPressed(SW2);
		}else{
			buttonShortPressed(SW2);
		}
	}
}
