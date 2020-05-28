# Author: Ayako Iwasaki
# Project: STEP2020
# reference:
#   https://github.com/xharaken/step2/blob/master/cache.py

import sys

# CacheElement is a data structure that stores information about a website.
# It includes
# - the contents of the website.
# - urls which are visited just before or after the access to the website.
# If it is the newest or oldest data in cache, next_url or prev_url is set to None, respectively.
# [Example]
# - contents: "bbb"
# - prev_url: "a.com"
# - next_url: "c.com"
class CacheElenemt:
  def __init__(self, prev_url, next_url, contents):
    self.contents = contents
    self.prev_url = prev_url
    self.next_url = next_url

# ALEXNOTE:  algorithm looks 
# Cache is a data structure that stores the most recently accessed N pages.
# In the dict, CashElement object is stored as a value where its url is the key.
# [Example]
# - dict: {{"a.com", CacheElement object for "a.com"}, ..., {"d.com", CacheElement object for "d.com"}}
# - oldest_url: "a.com" (the url which points to the oldest data in the cache)
# - newest_url: "d.com" (the url which points to the newest data in the cache)
class Cache:
  # Initializes the cache.
  # |n|: The size of the cache.
  def __init__(self, n):
    self.cache_size = n
    self.dict = {}
    self.newest_url = None
    # ALEXNOTE: did you forget to initialize self.oldest_url ? or does it not matter?

  # Access a page and update the cache so that it stores the most
  # recently accessed N pages. This needs to be done with mostly O(1).
  # |url|: The accessed URL
  # |contents|: The contents of the URL
  def access_page(self, url, contents):
    self.current_url = url
    # If the most recent accessed url in cache is the same as the current url, nothing needs to be done
    if self.newest_url == url:
      return

    # If the dict is empty, the first element is added
    if len(self.dict) == 0:
      self.dict[url] = CacheElenemt(None, None, contents)
      self.oldest_url = url

    # When the url is already in the dict
    elif self.dict.get(url):
      # If the current url is the oldest in the dict, the url that points the oldest data moves forward
      if self.oldest_url == url:
        self.oldest_url = self.dict[url].next_url
        self.dict[self.oldest_url].prev_url = None
      # If not, prev_url and next_url around the current url are updated
      else:
        self.dict[self.dict[url].prev_url].next_url = self.dict[url].next_url;
        self.dict[self.dict[url].next_url].prev_url = self.dict[url].prev_url;
      # Update the object of the current url as the most resently accessed url
      self.dict[self.newest_url].next_url = url
      
      // ALEXNOTE: can you cache  self.dict[url] in a local var and use it?  should be cheaper
      //                          than looking up the dictionary twice (remember O(1) )
      self.dict[url].prev_url = self.newest_url
      self.dict[url].next_url = None

    # When the url is not in the dict
    else:
      # If the dict is full, the oldest data is discarded
      if len(self.dict) == self.cache_size:
        self.prev_url_old = self.oldest_url;
        self.oldest_url = self.dict[self.prev_url_old].next_url
        self.dict[self.oldest_url].prev_url = None
        self.dict.pop(self.prev_url_old)
      # Add a new element 
      self.dict[url] = CacheElenemt(self.newest_url, None, contents)
      self.dict[self.newest_url].next_url = url

    self.newest_url = url

  # Return the URLs stored in the cache. The URLs are ordered
  # in the order in which the URLs are mostly recently accessed.
  def get_pages(self):
    out = []
    if len(self.dict) == 0:
      return out
    tmp = self.current_url
    while tmp:
      out.append(tmp)
      tmp = self.dict[tmp].prev_url
    return out

def cache_test():
  # Set the size of the cache to 4.
  cache = Cache(4)
  # Initially, no page is cached.
  equal(cache.get_pages(), [])
  # Access "a.com".
  cache.access_page("a.com", "AAA")
  # "a.com" is cached.
  equal(cache.get_pages(), ["a.com"])
  # Access "b.com".
  cache.access_page("b.com", "BBB")
  # The cache is updated to:
  #   (most recently accessed)<-- "b.com", "a.com" -->(least recently accessed)
  equal(cache.get_pages(), ["b.com", "a.com"])
  # Access "c.com".
  cache.access_page("c.com", "CCC")
  # The cache is updated to:
  #   (most recently accessed)<-- "c.com", "b.com", "a.com" -->(least recently accessed)
  equal(cache.get_pages(), ["c.com", "b.com", "a.com"])
  # Access "d.com".
  cache.access_page("d.com", "DDD")
  # The cache is updated to:
  #   (most recently accessed)<-- "d.com", "c.com", "b.com", "a.com" -->(least recently accessed)
  equal(cache.get_pages(), ["d.com", "c.com", "b.com", "a.com"])
  # Access "d.com" again.
  cache.access_page("d.com", "DDD")
  # The cache is updated to:
  #   (most recently accessed)<-- "d.com", "c.com", "b.com", "a.com" -->(least recently accessed)
  equal(cache.get_pages(), ["d.com", "c.com", "b.com", "a.com"])
  # Access "a.com" again.
  cache.access_page("a.com", "AAA")
  # The cache is updated to:
  #   (most recently accessed)<-- "a.com", "d.com", "c.com", "b.com" -->(least recently accessed)
  equal(cache.get_pages(), ["a.com", "d.com", "c.com", "b.com"])
  cache.access_page("c.com", "CCC")
  equal(cache.get_pages(), ["c.com", "a.com", "d.com", "b.com"])
  cache.access_page("a.com", "AAA")
  equal(cache.get_pages(), ["a.com", "c.com", "d.com", "b.com"])
  cache.access_page("a.com", "AAA")
  equal(cache.get_pages(), ["a.com", "c.com", "d.com", "b.com"])
  # Access "e.com".
  cache.access_page("e.com", "EEE")
  # The cache is full, so we need to remove the least recently accessed page "b.com".
  # The cache is updated to:
  #   (most recently accessed)<-- "e.com", "a.com", "c.com", "d.com" -->(least recently accessed)
  equal(cache.get_pages(), ["e.com", "a.com", "c.com", "d.com"])
  # Access "f.com".
  cache.access_page("f.com", "FFF")
  # The cache is full, so we need to remove the least recently accessed page "c.com".
  # The cache is updated to:
  #   (most recently accessed)<-- "f.com", "e.com", "a.com", "c.com" -->(least recently accessed)
  equal(cache.get_pages(), ["f.com", "e.com", "a.com", "c.com"])
  print("OK!")

# A helper function to check if the contents of the two lists is the same.
def equal(list1, list2):
  assert(list1 == list2)
  for i in range(len(list1)):
    assert(list1[i] == list2[i])

if __name__ == "__main__":
  cache_test()
