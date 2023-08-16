## Dictionary 

这是一本简单而通用的字典。实例化多个字典
构造函数。见下面的界面。

每个字典有1000个元素的空间。


### Overview about functions

create_dict: is a simple constructor for creating 
             a dictionary and setting up the 
             member field 'number_of_elements'
             and prepares the inner array 'elements'
``` c
Dictionary * create_dict(void);
```

add_item_label: adds item (void*) to the dictionary at given label 
                returns 0 if adding was sucessful otherwise -1
``` c
int add_item_label(Dictionary *,char label[],void *);
```

add_item_index: adds item (void*) to the dictionary at given index (int) 
                returns 0 if adding was sucessful otherwise -1
``` c
int add_item_index(Dictionary *, int index, void *);
```

get_element: returns the element at given label 
``` c
void * get_element_label(Dictionary *, char []);
```


get_element: returns the element at given index 
``` c
void * get_element_index(Dictionary *, int);
```


simple destructor function for avoiding memory leaks.
``` c
void destroy(Dictionary *);
```
