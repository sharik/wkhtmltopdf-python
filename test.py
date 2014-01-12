from pywkhtmltox import PDFConvertor

p = PDFConvertor(True)

p.set_option('out', '/tmp/1.pdf')
p.add_object({'page': 'http://google.com', 'load.debugJavascript': 'true'})


def calls(*args):
    print args

p.add_finished_callback(calls)


print p.convert()



