#ifndef CELL_HPP
#define CELL_HPP

class Cell {
public:
  Cell(bool alive = false);
  void setAlive(bool alive);
  bool isAlive() const;

private:
  bool alive;
};

#endif // CELL_HPP
