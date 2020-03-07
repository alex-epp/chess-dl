import collections


class State:
    def __init__(self, initial_state):
        self._state = initial_state
        self._on_change = collections.defaultdict(lambda: collections.defaultdict(set))

    def register_on_change(self, func, old_state=None, new_state=None):
        self._on_change[old_state][new_state].add(func)

    def change_to(self, new_state):
        self._state, old_state = new_state, self._state
        if old_state != new_state:
            for func in (self._on_change[None][None] |
                         self._on_change[None][new_state] |
                         self._on_change[old_state][None] |
                         self._on_change[old_state][new_state]):
                func(self._state, new_state)

    def is_at(self, state):
        return self._state == state
