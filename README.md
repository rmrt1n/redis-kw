# redis-kw
Clone of Redis (server & client), written in C, that supports a subset of the 
features in the original Redis. My attempt at #42SkillsChallenge no.9: In-memory
Database.

![Screeenshot of the client program](/screenshot.png)

## Built with

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)

## Build Instructions
Instructions to build the project from source.
```bash
# clone this repository
git clone https://github.com/rmrt1n/redis-kw.git && cd redis-kw

# cd into the repository
cd redis-kw

# build the redis server (rediskw)
make

# build the redis client (rediskw-cli)
make client

# run the unit tests
make test
```

## Usage
`rediskw` is the server's executable, while `rediskw-cli` is the client's. You
can run them separately or use the utility script `connect.sh` to run both at
once (useful when developing).
```bash
# run the utility script
chmod +x connect.sh && ./connect.sh

# run the server
./rediskw

# run the client
./rediskw-cli
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
- [x] decr       - [x] hkeys      - [x] lindex    - [x] smismember
- [x] incrby     - [x] hvals      - [x] lpos      - [ ] sdiff
- [x] decrby     - [x] hlen       - [x] lset      - [ ] sinter
- [x] strlen     - [ ] hincrby    - [x] lrem      - [ ] sunion
- [ ] append     - [x] hmget      - [x] lrange    - [ ] sdiffstore
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

## License
Distributed under the MIT License. See [LICENSE](/LICENSE) for more information.

