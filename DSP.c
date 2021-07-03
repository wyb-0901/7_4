void InitSysCtrl(void)
{
Uint16 i;
EALLOW;
SysCtrlRegs.WDCR=0x0068;     //禁止看门狗模块
SysCtrlRegs.PLLCR=0xA;    //初始化PLL模块,如果外部品振为30MHz,则SYSCLKOUT=
//30 MHz x *10/2=150MHz
for(i=0;i<5000;i++){}      //延时,使得PLL模块能够完成初始化操作
 //高速时钟预定标器和低速时钟预定标器,产生高速外设时钟HSPCLK和低速外设时钟LSPCLK
SysCtrlRegs.HISPCP.all=0x0001;    // HSPCLK=150MHz/2=75 MHz
SysCtr1Regs.LOSPCP.all=0x0002;  // LSPCLK=150MHz/4= 37.5MHz
EDIS;
}


void InitGpio(void)
{
EALLOW;
GpioMuxRegs.GPFMUX.bit.XF_GPIOF14=0;   //将XF引脚设置位I/0口GpioMuxRegs.GPFDIR.bit.GPIOF14=1;   //引脚方向为输出GpioDataRegs.GPFSET.bit.GPIOF14=1;          //引脚初始化为高电平,D1灭
	EDIS;
	}

void InitCpuTimers(void)
{
 CpuTimer0. RegsAddr=&CpuTimer0Regs; //使得CpuTimer0.RegsAddr指向定时器寄存器
CpuTimer0Regs.PRD.all=0xFFFFFF;        //初始化CpuTimer0的周期寄存器CpuTimer0Regs.TPR.all=0;             //初始化定时器预定标计数器
CpuTimer0Regs.TPRH.all=0;
CpuTimer0Regs.TCR.bit.TSS=1;        //停止定时器
CpuTimer0Regs.TCR.bit.TRB=1;      //将周期寄存器PRD中的值装入计数器寄存器TIM中
CpuTimer0.InterruptCount =0;       //初始化定时器中断计数器
   }

void ConfigCpuTimer(struct CPUTIMER_VARS * Timer，float Freq，float Period)
{
   Uint32 temp;
Timer - >CPUEreqInMHz =Freq;
Timer- >PeriodInUSec = Period;
temp = (long)(Freq* Period);
Timer - >RegsAddr- >PRD.all = temp;       //给定时器周期寄存赋值
Timer >RegsAddr- >TPR.a1l = 0;           //给定时器预定标寄存器赋值
Timer - >RegsAddr - > TPRH.all = 0;       
//初始化定时器控制寄存器:
Timer - >RegsAddr - >TCR.bit.TIF = 1;       //清除中断标志位
Timer- >RegsAddr - >TCR.bit.Tss = 1;	       //停止定时器
Timer- >RegsAddr- >TCR.bit.TRB= 1;       //定时器重装,将定时器周期寄存器的值装入定
//时器计数器寄存器
Timer - >RegsAddr- >TCR.bit.SOFT = 1 ;
Timer - >RegsAddr - >TCR.bit.FREE= 1;
Timer - >RegsAddr - >TCR. bit. TIE= 1;        //使能定时器中断
Timer- >InterruptCount = 0;                 //初始化定时器中断计数器
}

int sec=0,min=0,hour=0;
void main(void)
{
   InitSysCtrl( )﹔       //初始化系统函数
DINT;
IER= 0x0000;        //禁止CPU中断
IFR = 0x0000;        //清除CPU中断标志
InitPieCtrl( );         //初始化PIE控制寄存器
InitPieVectTable( );   //初始化PIE中断向量表
InitPeripherals( );     //初始化CPU定时器模块
InitGpio( );          //初始化GPIO
PieCtr1.PIEIER1. bit.INTx7 = 1;    //使能PIE模块中的CPU定时器0的中断
IER|= M_INT1 ;	          //开CPU中断1
EINT;      //使能全局中断
ERTM;    //使能实时中断
ConfigCpurimer( &CpuTimer0,150,100000);    //CPU定时器0的周期为0.1s
while(1)
{
    
    if()                                            //K1按下则启动定时器，开始计时
     StartCpuTimer0( );                             //启动CPU定时器0
    if()                                            //K2按下暂停计时
     StopCpuTimer0();
     if()                                           //K3按下清零时间
    {
        CpuTimer0.InterruptCount = 0;
        sec = 0;
        min = 0;
        hour = 0;
    }

}
}



interrupt void TINTO_ISR(void)          // CPU Timer0中断函数
{
  CpuTimer0.InterruptCount ++;
 if(CpuTimer0.InterruptCount == 10)
 {sec++;
 CpuTimer0.InterruptCount = 0;
  }                                //第10次进入中断为1秒,同时清除毫秒计数
  if(sec == 59)
  {min++;
  sec = 0;
  }                               //60秒记为1分，清除秒计数
  if(min == 59)
  {
      hour++;
  }
CpuTimer0Regs. TCR.bit. TIE= 1 ;     //清除定时器中断标志位
PieCtr1. PIEACK.bit. ACK1 = 1;        //响应同组其他中断
EINT;           //开全局中断  
}

   
