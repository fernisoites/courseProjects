import datetime
from haystack import indexes
from myapp.models import IndexTable


class NoteIndex(indexes.SearchIndex, indexes.Indexable):
    text = indexes.CharField(document=True, use_template=True)
    department = indexes.CharField(model_attr='department')
    sink = indexes.CharField(model_attr='sink')

    def get_model(self):
        return IndexTable

    def index_queryset(self, using=None):
        """Used when the entire index for model is updated."""
        return self.get_model().objects.filter(date__lte=datetime.datetime.now())
