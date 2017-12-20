#ifndef FILTER_H
#define FILTER_H

class Filter {

 public:

  Filter(int window);
  void AddItem(int item);
  bool GetValueEQ(int & item);
  int GetMoy();

  void Clear();
  int GetMoyOfNewVal(bool dump = false);

  ~Filter();

 private:
  int _window;
  int* _buffer;
  int _window_index;

};

#endif //FILTER_H
