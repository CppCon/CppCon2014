from ctypes import *

lib = CDLL("libhairpoll.dylib")
lib.hairpoll_construct.restype = c_void_p
lib.hairpoll_construct.argtypes = [c_char_p, c_void_p]

lib.hairpoll_destruct.restype = None
lib.hairpoll_destruct.argtypes = [c_void_p]

lib.hairpoll_add_option.restype = c_int
lib.hairpoll_add_option.argtypes = [c_void_p, c_char_p, c_char_p, c_void_p]

lib.hairpoll_vote.restype = None
lib.hairpoll_vote.argtypes = [c_void_p, c_int, c_void_p]

hairpoll_result_handler = CFUNCTYPE(None, c_void_p, c_char_p, c_int, c_char_p)
lib.hairpoll_tally.restype = None
lib.hairpoll_tally.argtypes = [c_void_p, hairpoll_result_handler, c_void_p, c_void_p]

hairpoll = lib.hairpoll_construct("Stefanus Du Toit", None)

skeletor = lib.hairpoll_add_option(hairpoll, "Skeletor",
                                   "http://static.comicvine.com/uploads/original/4/49448/2413657-skeletor.jpg", None)
beast = lib.hairpoll_add_option(hairpoll, "Beast Man",
                                "https://angryjedi.files.wordpress.com/2010/10/he-man_beast_man.jpeg", None)

lib.hairpoll_vote(hairpoll, skeletor, None)
lib.hairpoll_vote(hairpoll, beast, None)
lib.hairpoll_vote(hairpoll, beast, None)

def print_result(client_data, name, votes, html):
    print name, votes

lib.hairpoll_tally(hairpoll, hairpoll_result_handler(print_result), None, None)

lib.hairpoll_destruct(hairpoll)
