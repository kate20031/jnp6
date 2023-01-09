#ifndef WORLDCUP_DIE_H
#define WORLDCUP_DIE_H

// Reprezentuje interfejs kostki do gry.
// Konkretna implementacja zostanie dostarczona przy testach.
class Die {
public:
    virtual ~Die() = default;

    // Zwraca wynik rzutu kostką.
    [[nodiscard]] virtual unsigned short roll() const = 0;
};

#endif//WORLDCUP_DIE_H
