from __future__ import unicode_literals, absolute_import
import sys

sys.path.append('.')
# sys.path.append('env2/lib/python2.7/site-packages/')

def callback_finished(conv, *args, **kwargs):
    print "Callback finished", 
    print args, kwargs

def callback_error(conv, *args, **kwargs):
    print "Callback error",
    print args, kwargs

def callback_phase(conv, *args, **kwargs):
    print "Callback phase",
    print args, kwargs

def callback_progress(conv, *args, **kwargs):
    print "Callback progress",
    print args, kwargs

def callback_warning(conv, *args, **kwargs):
    print "Callback warning ", args, kwargs


from _pywkhtmltox import WkhtmltoxError, PDFConvertor, ImageConvertor, CALLBACK_ERROR, CALLBACK_PHASE, CALLBACK_FINISH, CALLBACK_PROGRESS, CALLBACK_WARNING
import _pywkhtmltox
print _pywkhtmltox.__dict__

conv = PDFConvertor(True)
conv.add_callback(callback_warning, CALLBACK_WARNING)
conv.add_callback(callback_error, CALLBACK_ERROR)
conv.add_callback(callback_progress, CALLBACK_PROGRESS)
conv.add_callback(callback_finished, CALLBACK_FINISH)
conv.add_callback(callback_phase, CALLBACK_PHASE)
conv.set_option('out', '/tmp/22.pdf')
conv.add_page({'page': 'http://google.com'})
print 'Result: ', conv.convert()







