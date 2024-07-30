// Copyright 2024 Nick Marino (github.com/nwmarino)

#ifndef STATIMC_REGISTER_H
#define STATIMC_REGISTER_H

class Register {
  const char *m_id;
  bool m_free;

  public:
    explicit inline Register(const char *id) : m_id(id), m_free(true) {}
    inline const char *get_id() { return m_id; }
    inline bool is_free() { return m_free; }
    inline void alloc() { m_free = false; }
    inline void free() { m_free = true; }
};

#endif  // STATIMC_REGISTER_H