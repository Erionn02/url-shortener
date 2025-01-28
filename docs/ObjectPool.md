---
generator: doxide
---


# ObjectPool

**template&lt;typename Object&gt; class ObjectPool**

This object pool is meant for use in multi-threaded environment where Object type is not a thread safe
Many threads can share ObjectPool and have a unique ownership of an instance of Object
with getObject call, which ensures that calling thread is the only owner of returned instance


