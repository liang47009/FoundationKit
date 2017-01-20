import requests
import sys
import os
import traceback
from contextlib import closing

class ProgressBar(object):  
    def __init__(self, title, count=0.0, run_status=None, fin_status=None, total=100.0,    unit='', sep='/', chunk_size=1.0):
        super(ProgressBar, self).__init__()
        self.info = "[%s] %s %.2f %s %s %.2f %s"
        self.title = title
        self.total = total
        self.count = count
        self.chunk_size = chunk_size
        self.status = run_status or ""
        self.fin_status = fin_status or " " * len(self.statue)
        self.unit = unit
        self.seq = sep

    def __get_info(self):
        # 【名称】状态 进度 单位 分割线 总数 单位  
        _info = self.info % (self.title, self.status, self.count/self.chunk_size, self.unit, self.seq, self.total/self.chunk_size, self.unit)
        return _info
  
    def refresh(self, count=1, status=None):
        self.count += count
        # if status is not None:  
        self.status = status or self.status
        end_str = "\r"
        if self.count >= self.total:
            end_str = '\n'
            self.status = status or self.fin_status
              
  
        """ 
        没搞懂 print(end="")的用法 
        ,在eric中打印的东西看不到 
        ,在window控制台下单条语句刷新并不添加新的行 
        """    
        # print(,end="")的用法,可能会出现打印看不到的情况  
        print(self.__get_info(), end=end_str, )

def throw_error(msg):
    raise Exception(msg)

def download(url, filePath):
    with closing(requests.get(url, stream=True)) as response:
        chunk_size = 1024
        content_size = int(response.headers['content-length'])
        """ 
        需要根据 response.status_code 的不同添加不同的异常处理 
        """  
        print('Size:', content_size,"byte", "Status:",response.status_code , )
        _, fileName=os.path.split(url)
        progress = ProgressBar(fileName
                    , total=content_size
                    , unit="KB"
                    , chunk_size=chunk_size
                    , run_status="正在下载"
                    , fin_status="下载完成")
            # chunk_size = chunk_size < content_size and chunk_size or content_size  
        with open(filePath, "wb") as file:
            for data in response.iter_content(chunk_size=chunk_size):
                file.write(data)
                progress.refresh(count=len(data))
def usage():
    print("====================================")
    print("Usage:")
    print("python ./downloader.py URL [SaveFullPath]\n")

def main():
    if len(sys.argv)<2:
        usage()
        throw_error("not enough values to unpack (expected 2, got 1)")
    URL = sys.argv[1]
    if len(sys.argv) == 3:
        download(URL, sys.argv[2])
    else:
        _, fileName=os.path.split(URL)
        download(URL, './'+fileName)

# -------------- main --------------
if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        traceback.print_exc()
        sys.exit(1)