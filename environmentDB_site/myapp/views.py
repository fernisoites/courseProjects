from django.shortcuts import render, render_to_response
from django.template import RequestContext
from myapp.models import IndexTable
from myapp.forms import IndexTableForm
from django.http import HttpResponse
from datetime import datetime


try:
    import json
except ImportError:
    import simplejson as json

from reportlab.pdfgen.canvas import Canvas
from reportlab.lib.pagesizes import letter
from reportlab.lib.units import cm, mm, inch, pica
from reportlab.graphics.barcode.code128 import Code128
from reportlab.graphics.barcode.qr import QrCodeWidget
from reportlab.graphics.shapes import Drawing
from reportlab.graphics import renderPDF
import qrcode


def index(request):
    # Query the database for a list of ALL categories currently stored.
    # Order the categories by no. likes in descending order.
    # Retrieve the top 5 only - or all if less than 5.
    # Place the list in our context_dict dictionary which will be passed to the template engine.
    entry_list = IndexTable.objects.order_by('-entry_id')[:100]

    context_dict = {'entries': entry_list}


    # Render the response and send it back!
    return render(request, 'myapp/index.html', context_dict)


def add_QRcode(request):
    # add a new set of QRcode, each QR is one sample.
    # date and time is also included in this QRcode.

    response = HttpResponse(content_type='application/pdf')
    response['Content-Disposition'] = 'attachment; filename="QRcodes.pdf"'

    #this is not a real QRList, just storing all text information for QRcode generation.
    QRList = []

    time = datetime.now().strftime("%I:%M%p on %B %d, %Y")

    for index in range(3182, 3198):
        # make a list of MICU, each element is either a sink or pTrap,
        # the time of QRcode generation was also included.
        dict = [time, 'MICU', str(index)]

        dict1 = dict+['sink']
        QRList.append(dict1)

        dict2 = dict+['pTrap']
        QRList.append(dict2)

    for index in range(5185, 5199):
        # make a list for SICU.
        dict = [time, 'SICU', str(index)]

        dict1 = dict+['sink']
        QRList.append(dict1)

        dict2 = dict+['pTrap']
        QRList.append(dict2)

        '''Generates book labels using the barcodes passed in. Each code should be
  an integer, and will be printed using 9 digits (padded with zeroes if necessary).
  Up to 30 labels will be generated per page for compatibility with Avery form
  5160/8160 or most other address label forms containing 30 per sheet. The text
  of a PDF document is returned (to be sent back to the browser).'''

    #using reportlab library, produce a canvas like pdf for drawing.
    pdf = Canvas(response, pagesize=letter)
    pdf.setFillColorRGB(0, 0, 0)

    # These 2 initial value was originally from labelgen.py from google code,
    # The value has been changed due to the use of Avery5160
    # The original value was 45 and 71.
    imgX = startX = 25
    imgY = startY = letter[1] - 101

    for singleQRCode in QRList:
        if imgX > 600:
            # Start a new page
            imgX = startX
            imgY = startY
            pdf.showPage()

        # Generate and draw the barcode
        curDate = singleQRCode[0]
        curUnit = singleQRCode[1]
        curRoom = singleQRCode[2]
        curLocation = singleQRCode[3]
        text = curDate+','+curUnit+','+curRoom+','+curLocation

        # QRPureImg is just the QRcode, not on canvas yet,
        # QRImage is canvas produced by reportlab
        QRPureImg = QrCodeWidget(text)
        bounds = QRPureImg.getBounds()
        width = bounds[2] - bounds[0]
        height = bounds[3] - bounds[1]
        QRImage = Drawing(45, 45, transform=[45. / width, 0, 0, 45. / height, 0, 0])
        QRImage.add(QRPureImg)
        renderPDF.draw(QRImage, pdf, imgX, imgY)

        #add text label for QRcode, the original value was for barcode, which his here
        # pdf.setFont("Courier", 12)
        # pdf.drawCentredString(imgX + 64.5, imgY - 10, txt)
        # pdf.setFont("Courier", 10)
        # pdf.drawCentredString(imgX + 64.5, imgY - 20, res['title'].upper()[:20].strip())

        pdf.setFont("Courier", 6)
        pdf.drawString(imgX + 60, imgY + 35, curDate)

        pdf.setFont("Courier", 12)
        pdf.drawString(imgX + 60, imgY + 20, curUnit+' '+curRoom)

        pdf.setFont("Courier", 12)
        pdf.drawString(imgX + 60, imgY + 5, curLocation)

        imgY -= 71  # Move down to the next label location

        if imgY < 50:
            # Start a new column
            imgY = startY
            imgX += 200

    pdf.showPage()
    pdf.save()

    return response

def about(request):
    return HttpResponse("To be added")


def add_category(request):
    # A HTTP POST?
    if request.method == 'POST':
        form = IndexTableForm(request.POST)

        # Have we been provided with a valid form?
        if form.is_valid():
            # Save the new category to the database.
            new_form = form.save(commit=False)

            organism_id_swab = form.swab_other()
            new_form.organism_id_swab = organism_id_swab
            new_form.save()

            organism_id_water = form.water_other()
            new_form.organism_id_water = organism_id_water
            new_form.save()

            # Now call the index() view.
            # The user will be shown the homepage.
            return index(request)

        else:
            # The supplied form contained errors - just print them to the terminal.
            print form.errors
    else:
        # If the request was not a POST, display the form to enter details.

        form = IndexTableForm(initial={
            'date': IndexTable.objects.order_by('-entry_id')[0].date,
            'sampler': IndexTable.objects.order_by('-entry_id')[0].sampler,
            'department': IndexTable.objects.order_by('-entry_id')[0].department})

    # Bad form (or form details), no form supplied...
    # Render the form with error messages (if any).

    return render(request, 'myapp/add_category.html', {'form': form})


def search(request):
    request_params = request.GET.get
    IndexTable_results = IndexTable.objects.filter(
        sink=request_params('sink', ''),
        department=request_params('department', ''))

    return render_to_response('myapp/search_results.html', {'results': IndexTable_results})



# Create your views here.
