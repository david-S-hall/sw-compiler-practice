#!/usr/bin/python                                                                                                                                                      
# -*- coding: utf-8 -*-
# author: 
from subprocess import Popen, PIPE
import select
import threading
import os
import time

def stdout_theard(end_mark, cur_stdout, stdout_lock, string_list):
  #用户获取subprocess的stdout输出的线程,防止阻塞
  #cur_stdout是一个file对象,end_mark是个随机字符串，获取到这个字符串表明结束
  #先暂停0.01秒
  time.sleep(0.01)
  for i in range(3000):
    try:
      out_put = cur_stdout.readline()
      if not out_put:
        #添加结束标记
        # stdout_lock.acquire()
        # string_list.append(end_mark)
        # stdout_lock.release()
        break
      if out_put == end_mark:
      #out put正好和end_mark相等的特殊情况
        continue
      #外部获取到指定内容会清理string_list列表，所以要加锁
      stdout_lock.acquire()
      string_list.append(out_put.rstrip().lstrip())
      print string_list
      stdout_lock.release()
      time.sleep(0.02)
    except:
      print 'wtffff!!!!!!tuichule !!'
      break

class IOServer(object):
  def __init__(self, args, server_env = None):
    if server_env:
      self.process = Popen(args, stdin=PIPE, stdout=PIPE, stderr=PIPE, env=server_env)
    else:
      self.process = Popen(args, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    
    self.stdout_list = []
    self.stdout_lock = threading.Lock()
    self.end_mark = 'end9c2nfxz'
    self.cur_thread = threading.Thread(target=stdout_theard, \
        args=(self.end_mark, self.process.stdout, self.stdout_lock, self.stdout_list))
    self.cur_thread.setDaemon('True')
    self.cur_thread.start()
    #flags = fcntl.fcntl(self.process.stdout, fcntl.F_GETFL)
    #fcntl.fcntl(self.process.stdout, fcntl.F_SETFL, flags | os.O_NONBLOCK)

  def send(self, data, tail='\r\n'):
    self.process.stdin.write(data+tail)
    self.process.stdin.flush()

  def recv(self, t=.01, stderr=0):
    r = None
    if len(self.stdout_list) > 0:
      self.stdout_lock.acquire()
      r = self.stdout_list[0]
      del self.stdout_list[0]
      self.stdout_lock.release()
    time.sleep(t)
    return r

  def status(self):
    if self.process.poll() is None:
      return True
    return False

  def close(self):
    self.process.kill()