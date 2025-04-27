
#include <vera_exp_task.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
//#include <vera_directc.h>
#include <tiff.h>
#include "c2vera.h"

static void call_back(void *cdata){

}

void write2vera(int reg, int val){
  VI_ArgType    args[2];
  VeraTask      TaskHandle;
  VeraTaskDecl  task_decl;
  VI_ExpTask    *ExpTaskHandle;
  VI_ValueP     vp;
  int           numOfArgs,i;

  numOfArgs = 2;
  for(i=0;i<2;i++){
    args[i].type = VI_INT;
    args[i].isVar = 0;
    args[i].size = 32;
  }

  ExpTaskHandle = vera_GetExportMethods();

  task_decl= (*(ExpTaskHandle->GetExportTask))("write_c2vera",numOfArgs,args);

  TaskHandle = (*(ExpTaskHandle->NewTask))(task_decl);

  vp.intVal = &reg;
  (*(ExpTaskHandle->SetArgument))(TaskHandle,0,vp);//set first arg value
  vp.intVal = &val;
  (*(ExpTaskHandle->SetArgument))(TaskHandle,1,vp);//set first arg value

  //call vera export task : TaskHandle->"write_c2vera"
  (*(ExpTaskHandle->CallTask))(TaskHandle,1,call_back,TaskHandle);

  (*(ExpTaskHandle->DeleteTask))(TaskHandle);

}

