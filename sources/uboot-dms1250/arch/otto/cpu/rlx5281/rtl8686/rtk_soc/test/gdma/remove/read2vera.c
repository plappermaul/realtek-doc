
#include <vera_exp_task.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
//#include <vera_directc.h>
#include <tiff.h>
#include "c2vera.h"

static void call_back(void *cdata){

}

uint32 read2vera(int reg){
  VI_ArgType    args[2];
  VeraTask      TaskHandle;
  VeraTaskDecl  task_decl;
  VI_ExpTask    *ExpTaskHandle;
  VI_ValueP     vp;
  int           numOfArgs,ival;

  numOfArgs = 2;
  args[0].type = VI_INT;
  args[0].isVar = 0;
  args[0].size = 32;
	args[1].type = VI_INT;
	args[1].isVar = 1;
	args[1].size = 32;

  ExpTaskHandle = vera_GetExportMethods();

  task_decl= (*(ExpTaskHandle->GetExportTask))("read_c2vera",numOfArgs,args);

  TaskHandle = (*(ExpTaskHandle->NewTask))(task_decl);

  vp.intVal = &reg;
  (*(ExpTaskHandle->SetArgument))(TaskHandle,0,vp);//set first arg value

  //call vera export task : TaskHandle->"read_c2vera"
  (*(ExpTaskHandle->CallTask))(TaskHandle,1,call_back,TaskHandle);

	vp.intVal = &ival;
	(*(ExpTaskHandle->FetchArgument))(TaskHandle,1,vp);
	
  (*(ExpTaskHandle->DeleteTask))(TaskHandle);

	return ival;
}

