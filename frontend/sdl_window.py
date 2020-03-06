import state
import board_renderer
import pygame
import configparser
import json
from os import path
import numpy as np
from typing import NamedTuple


class SDLWindow:
    def __init__(self, window_config, backbuffer_size=None):
        self._window_buffer = None

        self._window_config = window_config
        self.backbuffer_size = backbuffer_size or window_config.size

        pygame.display.init()
        self._setup_window()

        self._event_listeners = set()

    def _setup_window(self, *, size=None):
        window_flags = 0
        if self._window_config.resizable:
            window_flags |= pygame.RESIZABLE

        self._window_buffer = pygame.display.set_mode(size or self._window_config.size, window_flags)
        pygame.display.set_caption(self._window_config.title)

    def clear(self, color=(0, 0, 0)):
        self._window_buffer.fill(color)

    def _back_buffer_real_area(self):
        wi, hi = self.backbuffer_size
        ws, hs = self._window_buffer.get_size()
        ri, rs = wi / hi, ws / hs
        w, h = (wi * hs / hi, hs) if rs > ri else (ws, hi * ws / wi)
        x, y = (ws - w) / 2, (hs - h) / 2
        return pygame.Rect(x, y, w, h)

    def _backbuffer_rect_to_real_rect(self, area: pygame.Rect):
        bb_area = self._back_buffer_real_area()
        norm_pos = np.array(area.topleft) / self.backbuffer_size
        real_pos = norm_pos * bb_area.size + bb_area.topleft
        norm_size = np.array(area.size) / self.backbuffer_size
        real_size = norm_size * bb_area.size
        return pygame.Rect(real_pos, real_size)

    def _real_pos_to_backbuffer_pos(self, pos):
        bb_area = self._back_buffer_real_area()
        norm_pos = (np.array(pos) - bb_area.topleft) / bb_area.size
        return norm_pos * self.backbuffer_size

    def draw(self, surface: pygame.Surface, pos):
        r = self._backbuffer_rect_to_real_rect(pygame.Rect(pos, surface.get_size()))
        self._window_buffer.blit(pygame.transform.scale(surface, r.size), r.topleft)

    def register_event_listener(self, listener, type_=None):
        self._event_listeners.add((listener, type_))

    def get_events(self):
        for event in pygame.event.get():
            if event.type == pygame.VIDEORESIZE:
                self._setup_window(size=(event.w, event.h))

            # Scale mouse positions
            elif (event.type == pygame.MOUSEMOTION or
                  event.type == pygame.MOUSEBUTTONUP or
                  event.type == pygame.MOUSEBUTTONDOWN):
                event.pos = self._real_pos_to_backbuffer_pos(event.pos)

            for listener, type_ in self._event_listeners:
                if type_ is None or type_ == event.type:
                    listener(event)

            yield event

    def do_events(self):
        for _ in self.get_events():
            pass

    @property
    def window_buffer(self):
        return self._window_buffer

    def flip(self):
        pygame.display.flip()


class SDLSpritePack:
    def __init__(self, texture_file, tile_size, tile_mapping):
        self._texture_file = texture_file
        self._tile_size = np.array(tile_size)
        self._tile_mapping = {k: np.array(v) for k, v in tile_mapping.items()}
        self._texture = None

    @classmethod
    def from_file(cls, file):
        config = configparser.ConfigParser()
        config.optionxform = str
        config.read(file)
        texture_file = path.join(path.dirname(file), config['meta']['texture_file'])
        tile_size = json.loads(config['meta']['tile_size'])
        tile_mapping = {k: json.loads(v) for k, v in config['labels'].items()}
        return cls(texture_file, tile_size, tile_mapping)

    def load(self):
        self._texture = pygame.image.load(self._texture_file)
        self._texture.convert_alpha()

    def area(self, tile):
        return pygame.Rect(self._tile_mapping[tile], self.tile_size)

    def blit(self, tile, dest, pos):
        dest.blit(self.texture, pos, self.area(tile))

    @property
    def tile_size(self):
        return self._tile_size

    @property
    def texture(self):
        return self._texture


class SDLDragger:
    class Event:
        START = 0
        DRAG = 1
        END = 2

        def __init__(self, type_, start, drag, end=None):
            self.type = type_
            self.start = start
            self.drag = drag
            self.end = end

    def __init__(self, renderer):
        self._renderer = renderer
        self._drag_point = None
        self._dragging = False
        self._drag_start = None

        self._enabled = True
        self._event_listeners = set()

    def enable(self):
        self._enabled = True

    def disable(self):
        if self._dragging:
            self._dragging = False
            self._event(self.Event(self.Event.END, self._drag_start, self._drag_start))
        self._enabled = False

    def register_event_listener(self, listener):
        self._event_listeners.add(listener)

    def process_event(self, event: pygame.event.Event):
        if event.type == pygame.MOUSEMOTION:
            if self._dragging:
                self._drag_point = self._renderer.Coord(screen=event.pos)
                self._event(self.Event(self.Event.DRAG, self._drag_start, self._drag_point))
        elif event.type == pygame.MOUSEBUTTONDOWN:
            self._dragging = True
            self._drag_start = self._renderer.Coord(screen=event.pos)
            self._drag_point = self._drag_start
            self._event(self.Event(self.Event.START, self._drag_start, self._drag_point))
        elif event.type == pygame.MOUSEBUTTONUP:
            self._dragging = False
            drag_end = self._renderer.Coord(screen=event.pos)
            self._event(self.Event(self.Event.END, self._drag_start, self._drag_point, drag_end))

    def _event(self, event):
        if self._enabled:
            for l in self._event_listeners:
                l(event)


class Button:
    def __init__(self, surface: pygame.Surface, hover_surface: pygame.Surface, pressed_surface: pygame.Surface):
        self._surface = surface
        self._hover_surface = hover_surface
        self._pressed_surface = pressed_surface

        self._is_hovering = False
        self._is_pressing = False

        self._event_listeners = set()

    def process_event(self, event: pygame.event.Event):
        if event.type == pygame.MOUSEMOTION:
            self._is_hovering = self._surface.get_bounding_rect().collidepoint(event.pos)
        elif event.type == pygame.MOUSEBUTTONDOWN:
            self._is_pressing = self._is_hovering
        elif event.type == pygame.MOUSEBUTTONUP:
            if self._is_pressing:
                self._trigger_press_event()
            self._is_pressing = False

    def register_on_press(self, func):
        self._event_listeners.add(func)

    def surface(self):
        if self._is_hovering:
            if self._is_pressing:
                return self._pressed_surface
            else:
                return self._hover_surface
        else:
            return self._surface

    def _trigger_press_event(self):
        for listener in self._event_listeners:
            listener()

