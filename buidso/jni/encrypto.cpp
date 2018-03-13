//
// Created by fancy shen on 2018/3/12.
//

#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <elf.h>
#include <assert.h>
#include <sys/mman.h>


jstring getString(JNIEnv*) __attribute__((section (".mytext")));
jstring getString(JNIEnv* env){
    char ss[18]="IamtherightAnswer";
    for(int i=0;ss[i];++i){
        if(i%3==0){
            ss[i]='X';
        }
    }
    jstring temp=env->NewStringUTF(ss);
	return temp;
};

void init_getString() __attribute__((constructor));
unsigned long getLibAddr();

void init_getString(){
  char name[15];
  unsigned int text_off;
  unsigned int nblock;
  unsigned int nsize;
  unsigned long base;
  unsigned long text_addr;
  unsigned int i;
  Elf32_Ehdr *ehdr;
  Elf32_Shdr *shdr;
  
  //获取so的起始地址
  base = getLibAddr();
  
  //获取指定section的偏移值和size
  ehdr = (Elf32_Ehdr *)base;
  text_off=(ehdr->e_entry&0xffffff)>>8;
  text_addr = text_off + base;
  
  nblock = ehdr->e_entry >> 24;
  nsize = ehdr->e_entry & 0xff;

  __android_log_print(ANDROID_LOG_INFO, "JNITag", "nblock =  0x%x,nsize:%d", nblock,nsize);
  __android_log_print(ANDROID_LOG_INFO, "JNITag", "base =  0x%lx", text_addr);
  printf("nblock = %d\n", nblock);
  
  //修改内存的操作权限
  if(mprotect((void *) (text_addr / PAGE_SIZE * PAGE_SIZE), 4096 * nsize, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
    puts("mem privilege change failed");
     __android_log_print(ANDROID_LOG_INFO, "JNITag", "mem privilege change failed");
  }
  //解密
  for(i=0;i< nblock; i++){  
    char *addr = (char*)(text_addr + i);
    *addr = ~(*addr);
  }
  
  if(mprotect((void *) (text_addr / PAGE_SIZE * PAGE_SIZE), 4096 * nsize, PROT_READ | PROT_EXEC) != 0){
    puts("mem privilege change failed");
  }
  puts("Decrypt success");
}

unsigned long getLibAddr(){
  unsigned long ret = 0;
  char name[] = "libencrypto.so";
  char buf[4096], *temp;
  int pid;
  FILE *fp;
  pid = getpid();
  sprintf(buf, "/proc/%d/maps", pid);
  fp = fopen(buf, "r");
  if(fp == NULL)
  {
    puts("open failed");
    goto _error;
  }
  while(fgets(buf, sizeof(buf), fp)){
    if(strstr(buf, name)){
      temp = strtok(buf, "-");
      ret = strtoul(temp, NULL, 16);
      break;
    }
  }
_error:
  fclose(fp);
  return ret;
}


jstring teststring(JNIEnv* env, jobject thiz)  
{  
    return getString(env);
} 

static JNINativeMethod gMethods[] = {  
    {"teststring", "()Ljava/lang/String;", (jstring*)teststring},//绑定  
};

static int registerNativeMethods(JNIEnv* env  
        , const char* className  
        , JNINativeMethod* gMethods, int numMethods) {  
    jclass clazz;  
    clazz = env->FindClass(className);  
    if (clazz == NULL) {  
        return JNI_FALSE;  
    }  
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {  
        return JNI_FALSE;  
    }  
  
    return JNI_TRUE;  
}  
static int registerNatives(JNIEnv* env) {  
    const char* kClassName = "com/fancy/testsoshell/MainActivity";//指定要注册的类  
    return registerNativeMethods(env, kClassName, gMethods,  
            sizeof(gMethods) / sizeof(gMethods[0]));  
} 

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {  
    JNIEnv* env = NULL;  
    jint result = -1;  
  
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {  
        return -1;  
    }  
    assert(env != NULL);  
  
    if (!registerNatives(env)) {//注册  
        return -1;  
    }  
    //成功  
    result = JNI_VERSION_1_4;  
  
    return result;  
}