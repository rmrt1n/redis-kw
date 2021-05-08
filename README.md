# redis-kw
A redis clone written in C. This is my attempt at #42SkillsChallenge no. 9: In-memory Database.
## Building
```
$ git clone https://github.com/rmrt1n/redis-kw.git && cd redis-kw
$ make
$ make test # for tests
$ ./connect.sh # connect script for now
```
## Commands supported
```
cmds list:
---
str cmds:        hash cmds:       list cmds:      set cmds:  
- [x] set        - [x] hset       - [x] lpush     - [x] sadd 
- [x] get        - [x] hget       - [x] rpush     - [x] srem 
- [x] mset       - [x] hdel       - [x] lpop      - [x] smembers  
- [x] mget       - [x] hgetall    - [x] rpop      - [x] sismember  
- [x] incr       - [x] hexists    - [x] llen      - [ ] scard  
- [x] decr       - [x] hkeys      - [x] lindex    - [ ] smismember
- [x] incrby     - [x] hvals      - [ ] lpos      - [ ] sdiff
- [x] decrby     - [x] hlen       - [x] lset      - [ ] sinter
- [x] strlen     - [ ] hincrby    - [x] lrem      - [ ] sunion
- [ ] append     - [ ] hmget      - [ ] lrange    - [ ] sdiffstore
- [ ] setrange   - [ ] hstrlen    - [ ] lpushx    - [ ] sinterstore
- [ ] getrange   - [ ] hsetnx     - [ ] rpushx    - [ ] sunionstore
- [ ] setnx      - [ ]            - [ ] ltrim     - [ ]
- [ ] msetnx                      - [ ]           
- [ ] 


keys cmds:      etc:
- [x] del       - [ ] ping
- [x] exists    - [x] quit
- [x] type      - [x] shutdown
- [ ] rename    - [ ] 
- [ ]
```
### License
[MIT](LICENSE)

