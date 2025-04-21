class State(object):
    def __init__(self):
        print("Processing current state:", str(self))
    def on_event(self, event):
        pass
    def __repr__(self):
        return self.__str__()
    def __str__(self):
        return self.__class__.__name__
    
class Idle(State):
    def on_event(self, event):
        if event == 'no_search':
            return Idle()
        elif event == 'search':
            return Seek()
        return self

class Seek(State):
     def on_event(self, event):
        if event == (not obj_located) and (not target_located):
            return Seek()

        elif event == obj_located and target_located:
            return Approach()
        
        return self
     
class Approach(State):
    # def on_event(self, event):
    #     if event == 'pin_entered':
    #         return UnlockedState()
    #     return self

class Grip(State):
    def on_event(self, event):
        if event == 'pressure_receivedressure_received':
        # supposed to receive a value called pressure
        # for this one should receive a pressure value from embedded
            return Liftift()
        return self
    # fail case: should stop/cancel in every state. But more steps later on. to cancel and stops right now but not sure yet


class Lift(State):
    # def on_event(self, event):
    #     if event == 'pin_entered':
    #         return UnlockedState()
    #     return self
class MoveToDrop(State):
    # def on_event(self, event):
    #     if event == 'pin_entered':
    #         return UnlockedState()
    #     return self
class Release(State):
    # def on_event(self, event):
    #     if event == 'pin_entered':
    #         return UnlockedState()
    #     return self