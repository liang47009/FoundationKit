#启用 Address Sanitizer
LOCAL_CPPFLAGS += -fsanitize=address -fno-omit-frame-pointer -O1
LOCAL_LDFLAGS  += -fsanitize=address -O1

LOCAL_SANITIZE:=unsigned-integer-overflow signed-integer-overflow
LOCAL_SANITIZE_DIAG:=unsigned-integer-overflow signed-integer-overflow

#隐藏不需要导出的函数，若某个函数需要导出，则添加JNIEXPORT或者__attribute__ ((visibility ("default")))即可。
LOCAL_CPPFLAGS += -fvisibility=hidden

#-ffunction-sections 和 -fdata-sections 将每个函数或符号创建为一个sections，
#其中每个sections名与function或data名保持一致。而在链接阶段， -Wl,–gc-sections 
#指示链接器去掉不用的section（其中-wl, 表示后面的参数 -gc-sections 传递给链接器），
#这样就能减少最终的可执行程序的大小了。
LOCAL_CPPFLAGS += -ffunction-sections -fdata-sections
LOCAL_LDFLAGS += -Wl,--gc-sections

#导出依赖库
#LOCAL_EXPORT_STATIC_LIBRARIES+= cpufeatures
#包含整个静态库
#LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures

#引用已经编译好的模块
include $(CLEAR_VARS)
LOCAL_MODULE    := libfoundationkit
LOCAL_SRC_FILES := ThirdParty/android/libs/$(TARGET_ARCH_ABI)/libfoundationkit.so
include $(PREBUILT_SHARED_LIBRARY)