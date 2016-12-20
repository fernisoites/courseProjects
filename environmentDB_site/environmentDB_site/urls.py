from django.conf.urls import patterns, include, url
from django.contrib import admin

urlpatterns = patterns('',
    # Examples:
    url(r'^$', 'myapp.views.index', name='index'),
    # url(r'^blog/', include('blog.urls')),
    url(r'^myapp/add_category/$', 'myapp.views.add_category', name='add_category'),
    
    url(r'^myapp/about/$', 'myapp.views.about', name='about'),
    url(r'^myapp/search/$', 'myapp.views.search'),
    url(r'^search/', include('haystack.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^myapp/add_QRcode', 'myapp.views.add_QRcode'),
)
