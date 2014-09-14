from __future__ import unicode_literals, absolute_import
import sys

sys.path.append('.')
# sys.path.append('env2/lib/python2.7/site-packages/')

import _pywkhtmltox
print 'AAAAAAAAAAAa'

#print _pywkhtmltox.__dict__
#print "BBBBBBBBBBBBBBBBB"

c = _pywkhtmltox.PDFConvertor(True)
print "CCCCCCCCCCCCCCCCC"

c.register_callback()
print "DDDDDDDDDDDDDD"


# convertor = PDFConvertor()
# convertor.settings.margin = (0, 1, 2, 2)
# convertor.settings.pageSize = 'A4'
#
# convertor.add_page('ya.ru')
# convertor.convert('/tmp/1.pdf')




