# ZRaft
ZRaft is a  distributed storage, supported by raft protocol. To learn more about raft, you can read this paper https://raft.github.io/raft.pdf. The storage engine of ZRaft db is rocksdb.

#Phase I ##DONE

implement raft protocol

#Phase II ##DONE

use rocksdb as storage engine

#Phase III ##TODO

For snapshot, generate snapshot of data by `checkpoint` of rocksdb and store the index, then send `checkpoint` to follower. After successfully send data, set the nextIndex of leader and index of follower.

To implement chunk balance, TODO...


#Phase IV

implement membership change. Add `update conf` command  in zraft, so the membership problem is similarly solved by raft protocol. 

#Phase V

To implement chunk balance, TODO...

Note that, to complie and run zraft, you should first install dynamic rocksdb lib, which can be found in https://github.com/facebook/rocksdb
