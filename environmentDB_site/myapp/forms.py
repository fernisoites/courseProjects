from __future__ import unicode_literals

from django import forms
from myapp.models import IndexTable

import datetime
import re
from re import escape
from django.conf import settings
from django.forms.widgets import Select, Widget
from django.forms. util import flatatt
from django.utils import datetime_safe, six
from django.utils.dates import MONTHS
from django.utils.encoding import force_str, force_unicode
from django.utils.formats import get_format
from django.utils.safestring import mark_safe
from django.utils.html import conditional_escape
from itertools import chain


__all__ = ('SelectDateWidget',)

RE_DATE = re.compile(r'(\d{4})-(\d\d?)-(\d\d?)$')

'''
class Select(Widget):
    def __init__(self, attrs=None, choices=()):
        super(Select, self).__init__(attrs)
        # choices can be any iterable, but we may need to render this widget
        # multiple times. Thus, collapse it into a list so it can be consumed
        # more than once.
        self.choices = list(choices)

    def render(self, name, value, attrs=None, choices=()):
        if value is None: value = ''
        final_attrs = self.build_attrs(attrs, name=name)
        output = [u'<select%s>' % flatatt(final_attrs)]
        options = self.render_options(choices, [value])
        if options:
            output.append(options)
        output.append('</select>')
        return mark_safe(u'\n'.join(output))

    def render_options(self, choices, selected_choices):
        def render_option(option_value, option_label):
            option_value = force_unicode(option_value)
            selected_html = (option_value in selected_choices) and u' selected="selected"' or ''
            return u'<option value="%s"%s>%s</option>' % (
                escape(option_value), selected_html,
                conditional_escape(force_unicode(option_label)))
        # Normalize to strings.
        selected_choices = set([force_unicode(v) for v in selected_choices])
        output = []
        for option_value, option_label in chain(self.choices, choices):
            if isinstance(option_label, (list, tuple)):
                output.append(u'<optgroup label="%s">' % escape(force_unicode(option_value)))
                for option in option_label:
                    output.append(render_option(*option))
                output.append(u'</optgroup>')
            else:
                output.append(render_option(option_value, option_label))
        return u'\n'.join(output)

'''

def _parse_date_fmt():
    fmt = get_format('DATE_FORMAT')
    escaped = False
    for char in fmt:
        if escaped:
            escaped = False
        elif char == '\\':
            escaped = True
        elif char in 'Yy':
            yield 'year'
        elif char in 'bEFMmNn':
            yield 'month'
        elif char in 'dj':
            yield 'day'
            
class SelectDateWidget(Widget):
    """
    A Widget that splits date input into three <select> boxes.

    This also serves as an example of a Widget that has more than one HTML
    element and hence implements value_from_datadict.
    """
    none_value = (0, '---')
    month_field = '%s_month'
    day_field = '%s_day'
    year_field = '%s_year'

    def __init__(self, attrs=None, years=None, months=None, empty_label=None):
        self.attrs = attrs or {}

        # Optional list or tuple of years to use in the "year" select box.
        if years:
            self.years = years
        else:
            this_year = datetime.date.today().year
            self.years = range(this_year, this_year + 10)

        # Optional dict of months to use in the "month" select box.
        if months:
            self.months = months
        else:
            self.months = MONTHS

        # Optional string, list, or tuple to use as empty_label.
        if isinstance(empty_label, (list, tuple)):
            if not len(empty_label) == 3:
                raise ValueError('empty_label list/tuple must have 3 elements.')

            self.year_none_value = (0, empty_label[0])
            self.month_none_value = (0, empty_label[1])
            self.day_none_value = (0, empty_label[2])
        else:
            if empty_label is not None:
                self.none_value = (0, empty_label)

            self.year_none_value = self.none_value
            self.month_none_value = self.none_value
            self.day_none_value = self.none_value

    def render(self, name, value, attrs=None):
        try:
            year_val, month_val, day_val = value.year, value.month, value.day
        except AttributeError:
            year_val = month_val = day_val = None
            if isinstance(value, six.string_types):
                if settings.USE_L10N:
                    try:
                        input_format = get_format('DATE_INPUT_FORMATS')[0]
                        v = datetime.datetime.strptime(force_str(value), input_format)
                        year_val, month_val, day_val = v.year, v.month, v.day
                    except ValueError:
                        pass
                else:
                    match = RE_DATE.match(value)
                    if match:
                        year_val, month_val, day_val = [int(v) for v in match.groups()]
        html = {}
        choices = [(i, i) for i in self.years]
        html['year'] = self.create_select(name, self.year_field, value, year_val, choices, self.year_none_value)
        choices = list(six.iteritems(self.months))
        html['month'] = self.create_select(name, self.month_field, value, month_val, choices, self.month_none_value)
        choices = [(i, i) for i in range(1, 32)]
        html['day'] = self.create_select(name, self.day_field, value, day_val, choices, self.day_none_value)

        output = []
        for field in _parse_date_fmt():
            output.append(html[field])
        return mark_safe('\n'.join(output))

    def id_for_label(self, id_):
        for first_select in _parse_date_fmt():
            return '%s_%s' % (id_, first_select)
        else:
            return '%s_month' % id_

    def value_from_datadict(self, data, files, name):
        y = data.get(self.year_field % name)
        m = data.get(self.month_field % name)
        d = data.get(self.day_field % name)
        if y == m == d == "0":
            return None
        if y and m and d:
            if settings.USE_L10N:
                input_format = get_format('DATE_INPUT_FORMATS')[0]
                try:
                    date_value = datetime.date(int(y), int(m), int(d))
                except ValueError:
                    return '%s-%s-%s' % (y, m, d)
                else:
                    date_value = datetime_safe.new_date(date_value)
                    return date_value.strftime(input_format)
            else:
                return '%s-%s-%s' % (y, m, d)
        return data.get(name, None)

    def create_select(self, name, field, value, val, choices, none_value):
        if 'id' in self.attrs:
            id_ = self.attrs['id']
        else:
            id_ = 'id_%s' % name
        if not self.is_required:
            choices.insert(0, none_value)
        local_attrs = self.build_attrs(id=field % id_)
        s = Select(choices=choices)
        select_html = s.render(field % name, val, local_attrs)
        return select_html


class IndexTableForm(forms.ModelForm):


    dep_set = (('MICU','MICU',),('SICU','SICU',))

    org_set =    ((' ',' ',),
        ('Serratia marcescens','Serratia marcescens',),
        ('Klebsiella oxytoca','Klebsiella oxytoca',),
        ('Klebsiella pneumoniae','Klebsiella pneumoniae',),
        ('Aeromonas spp.','Aeromonas spp.',),
        ('Raoultella plantiicola','Raoultella plantiicola',),
        ('Raoultella orninolytica','Raoultella orninolytica',),
        ('Citrobacter freundii','Citrobacter freundii',),
        ('Citrobacter amalonaticus','Citrobacter amalonaticus',),
        ('Enterobacter aerogenes','Enterobacter aerogenes',),
        ('Enterobacter cloacae','Enterobacter cloacae',),
        ('Enterobacter asburiae','Enterobacter asburiae',),
        ('Kluyvera intermedia','Kluyvera intermedia',),
        ('Pantoea spp.','Pantoea spp.',),
        ('Escherichia coli','Escherichia coli',),
        ('Proteus mirabilis','Proteus mirabilis',),
        ('Yokenella regensburgei','Yokenella regensburgei',),
        ('Enterobacteriaceae NO','Enterobacteriaceae NO',),
        ('other','other',))

    sink_set = ((' ',' ',),('pt', 'Pt',), ('nonPt', 'Non_Pt',))

    ind_set = ((' ',' ',),('pos', 'pos',), ('neg', 'neg' ,))

    agar_set = ((' ',' ',),('blue', 'blue',), ('cream', 'cream' ,)
                , ('green', 'green' ,), ('other', 'other' ,))

    loc_set = []
    loc_set_MICU = []
    loc_set_SICU = []
    loc_set.append((' ',' '))
    for i in range(3182,3198):
        loc_set_MICU.append((str(i),i))
        loc_set.append((str(i),i))
    for i in range(5185,5199):
        loc_set_SICU.append((str(i),i))
        loc_set.append((str(i),i))
    loc_set_MICU = tuple (loc_set_MICU)
    loc_set_SICU = tuple (loc_set_SICU)
    loc_set = tuple( loc_set)
   
    sampler = forms.CharField(help_text = "Sampler Name")
    date = forms.DateField(widget=SelectDateWidget, help_text = "Collect Date")
    department = forms.ChoiceField(widget=forms.Select, choices = dep_set, help_text="Department")
    location = forms.ChoiceField(widget=forms.Select, choices = loc_set, help_text="Location")
    sink = forms.ChoiceField(widget=forms.Select, choices = sink_set,  help_text="Sink", required=False)
    drain_swab_chromagar = forms.ChoiceField(widget=forms.Select, choices = agar_set,   help_text = "drain_swab_chromagar" , required=False)
    indirect_pos_neg = forms.ChoiceField(widget=forms.Select, choices = ind_set,   help_text = "indirect_pos_neg" ,required=False)
    organism_id_swab = forms.ChoiceField(widget=forms.Select, choices = org_set,  help_text = "organism_id_swab" , required=False)
    organism_id_swab_other = forms.CharField(help_text="Please specify the specy if choose OTHER above",required=False)
    #cav_drain_swab = forms.IntegerField(help_text = "cav_drain_swab" , required=False)
    #drain_entry_id is not included as it's autofilled in database, there is no need to change it.
    #likes = forms.IntegerField(widget=forms.HiddenInput(), initial=0)
    #slug = forms.CharField(widget=forms.HiddenInput(), required=False)

    ptrap_water_chromagar = forms.ChoiceField(widget=forms.Select, choices = agar_set,   help_text = "ptrap_water_chromagar" , required=False)
    indirect_pos_neg_drain = forms.ChoiceField(widget=forms.Select, choices = ind_set,  help_text = "indirect_pos_neg_drain" , required=False)
    organism_id_water = forms.ChoiceField(widget=forms.Select, choices = org_set,   help_text = "organism_id_water" , required=False)
    #cav_ptrap_water = forms.IntegerField(help_text = "cav_drain_swab" , required=False)
    organism_id_water_other = forms.CharField(help_text="Please specify the specy if choose OTHER above",required=False)

    def swab_other(self):
        organism_id_swab = self.cleaned_data['organism_id_swab']
        if organism_id_swab.upper() == 'OTHER':
            self.cleaned_data['organism_id_swab'] = self.cleaned_data['organism_id_swab_other']
        return self.cleaned_data['organism_id_swab']

    def water_other(self):
        organism_id_water = self.cleaned_data['organism_id_water']
        if organism_id_water.upper() == 'OTHER':
            self.cleaned_data['organism_id_water'] = self.cleaned_data['organism_id_water_other']
        return self.cleaned_data['organism_id_water']
    
    # An inline class to provide additional information on the form.
    class Meta:
        # Provide an association between the ModelForm and a model
        model = IndexTable
        fields = ('sampler',
                  'date',
                  'department',
                  'location',
                  'sink',
                  'drain_swab_chromagar',
                  'indirect_pos_neg',
                  'organism_id_swab',
                  'organism_id_swab_other',
                  'ptrap_water_chromagar',
                  'indirect_pos_neg_drain',
                  'organism_id_water',
                  'organism_id_water_other',
                
                  )

