from wkhtmltox import PDFConvertor

p = PDFConvertor(True)

p.set_option('/tmp/1.pdf')
p.add_object({'page': 'http://google.com', 'load.debugJavascript': 'true'})

print p.convert()



